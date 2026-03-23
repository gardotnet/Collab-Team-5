#include "BattleEnemyEntity.h"

#include "BattlePlayerEntity.h"
#include "Terminal_Recall/EncounterRelatedCode/BattleManager.h"
#include "Terminal_Recall/GridCellClasses/BattleGridCell.h"
#include "Terminal_Recall/StructsAndEnums/ForDataTable/EnemyInfoStruct.h"

ABattleEnemyEntity::ABattleEnemyEntity()
{
	PrimaryActorTick.bCanEverTick = true;
}
void ABattleEnemyEntity::OnTurnGo()
{
	TArray<int32> attacksThatCanHit;
	TMap<FVector, bool> searchedLoc;
	PlayerRef = nullptr;
	// checks if player can be hit
	for (int i = 0; i < AttackInfoList.Num(); i++)
	{
		if (AttackInfoList[i].Cooldown > 0)
			continue;
		for (const auto& cellInfo : AttackInfoList[i].ActionRange.Cells)
		{
			FVector rayStartPos = GetActorLocation() + (FVector(cellInfo.GridPos.X, cellInfo.GridPos.Y, 1) * 100.0f);
			FVector rayEndPos = rayStartPos + FVector(0.0f, 0.0f, -500.0f);
			FHitResult hitResult;
			FCollisionQueryParams params;
			if (bool* playerFoundOnThatCell = searchedLoc.Find(rayStartPos))
			{
				if (*playerFoundOnThatCell)
					attacksThatCanHit.Add(i);
			}
			else
			{
				GetWorld()->LineTraceSingleByChannel(hitResult, rayStartPos, rayEndPos, ECC_GameTraceChannel2, params);
				if (hitResult.bBlockingHit)
				{
					PlayerRef = Cast<ABattlePlayerEntity>(hitResult.GetActor());
					if (IsValid(PlayerRef))
					{
						UE_LOG(LogTemp, Warning, TEXT("I found %s"), *hitResult.GetActor()->GetActorNameOrLabel());
						attacksThatCanHit.Add(i);
						searchedLoc.Add(rayStartPos, true);
						break;
					}
				}
				searchedLoc.Add(rayStartPos, false);
			}
			for (const auto& followUpDirInfo : cellInfo.DirectionModifier)
			{
				FVector nextDir = FVector(0, 0, 0);
				// gets direction for follow-up cells
				switch (followUpDirInfo.Direction)
				{
				case ECompassDirection::N:
					nextDir = FVector(0, 100, 0);
					UE_LOG(LogTemp, Warning, TEXT("North"));
					break;
				case ECompassDirection::NE:
					nextDir = FVector(100, 100, 0);
					UE_LOG(LogTemp, Warning, TEXT("North East"));
					break;
				case ECompassDirection::E:
					nextDir = FVector(100, 0, 0);
					UE_LOG(LogTemp, Warning, TEXT("E"));
					break;
				case ECompassDirection::SE:
					nextDir = FVector(100, -100, 0);
					UE_LOG(LogTemp, Warning, TEXT("SE"));
					break;
				case ECompassDirection::S:
					nextDir = FVector(0, -100, 0);
					UE_LOG(LogTemp, Warning, TEXT("S"));
					break;
				case ECompassDirection::SW:
					nextDir = FVector(-100, -100, 0);
					UE_LOG(LogTemp, Warning, TEXT("SW"));
					break;
				case ECompassDirection::W:
					nextDir = FVector(-100, 0, 0);
					UE_LOG(LogTemp, Warning, TEXT("W"));
					break;
				case ECompassDirection::NW:
					nextDir = FVector(-100, 100, 0);
					UE_LOG(LogTemp, Warning, TEXT("NW"));
					break;
				default:
					continue;
				}
				FVector prevPos = rayStartPos;
				// until it cant continue in that direction or maximum range reached
				int32 totalFollowUp = 0;
				// get follow-up cell
				while (true)
				{
					// if it reached its range limit
					if (followUpDirInfo.Magnitude != 0 && totalFollowUp >= followUpDirInfo.Magnitude)
						break;
					prevPos += nextDir;
					rayEndPos = prevPos + FVector(0.0f, 0.0f, -500.0f);
					GetWorld()->LineTraceSingleByChannel(hitResult, prevPos, rayEndPos, ECC_GameTraceChannel2, params);
					if (hitResult.bBlockingHit)
					{
						PlayerRef = Cast<ABattlePlayerEntity>(hitResult.GetActor());
						if (IsValid(PlayerRef))
						{
							attacksThatCanHit.Add(i);
							searchedLoc.Add(prevPos, true);
							break;
						}
					}
					GetWorld()->LineTraceSingleByChannel(hitResult, prevPos, rayEndPos, ECC_GameTraceChannel1, params);
					if (hitResult.bBlockingHit)
					{
						searchedLoc.Add(prevPos, false);
						totalFollowUp++;
					}
					else
						break;
				}
				if (!attacksThatCanHit.IsEmpty() && attacksThatCanHit.Last() == i)
					break;
			}
			if (!attacksThatCanHit.IsEmpty() && attacksThatCanHit.Last() == i)
				break;
		}
	}
	if (attacksThatCanHit.Num() > 0)
	{
		int32 randomInt = FMath::RandRange(1, 100);
		if (randomInt < 30)
		{
			UE_LOG(LogTemp, Warning, TEXT("Nah i move"))
			StartMove(FVector::ZeroVector);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Should have avoided my range"))
			PlayerRef->CustomTakeDamage(1.0 * DamageMultiplier);
			OnActionDoneEvent.Broadcast();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Moving"))
		StartMove(FVector::ZeroVector);
	}
	// Decrement all cooldowns by 1
}

void ABattleEnemyEntity::StartMove(FVector position)
{
	if (!IsValid(PlayerRef))
		PlayerRef = BattleManager->PlayerRef;
	FIntPoint playerPos = PlayerRef->GetPos();
	UE_LOG(LogTemp, Warning, TEXT("Player pos = (%d, %d)"), playerPos.X, playerPos.Y);
	UE_LOG(LogTemp, Warning, TEXT("Enemy pos = (%d, %d)"), GridPos.X, GridPos.Y);
	FIntPoint direction = FIntPoint(FMath::Clamp( playerPos.X - GridPos.X, -1, 1),FMath::Clamp(playerPos.Y - GridPos.Y, -1, 1));
	UE_LOG(LogTemp, Warning, TEXT("Player Dir = (%d, %d)"), direction.X, direction.Y);
	OnActionDoneEvent.Broadcast();
}

void ABattleEnemyEntity::OnBattleStart(ABattleManager* battleManager, int32 enemyID)
{
	BattleManager = battleManager;
	FString enemyIDString = FString::FromInt(enemyID);
	if (!EnemyDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Where the enemy data table?"))
		return;
	}
	if (FEnemyInfoStruct* enemyInfo = EnemyDataTable->FindRow<FEnemyInfoStruct>(FName(*enemyIDString), TEXT("Entity Search")))
	{
		FindAndSaveActions(enemyInfo->AttackIdList, enemyInfo->MovementIdList);
		CurrentHealth = enemyInfo->Health;
		MaxHealth = enemyInfo->Health;
		DamageMultiplier = 1;
	}
}