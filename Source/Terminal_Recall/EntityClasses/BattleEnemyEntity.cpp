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
	UE_LOG(LogTemp, Warning, TEXT("Enemies turn"))
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
					if (bool* playerFoundOnThatCell = searchedLoc.Find(prevPos))
					{
						if (*playerFoundOnThatCell)
							attacksThatCanHit.Add(i);
						break;
					}
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
	FIntPoint direction = FIntPoint(FMath::Clamp( playerPos.X - GridPos.X, -1, 1),FMath::Clamp(playerPos.Y - GridPos.Y, -1, 1));
	TArray<ABattleGridCell*> importantCells;
	TArray<ABattleGridCell*> offTargetCell;
	TArray<FVector> searchedLoc;
	
	for (const auto& moveActions : MoveInfoList)
	{
		for (const auto& cellInfo : moveActions.ActionRange.Cells)
		{
			// get pos, dir and set up raycasting
			FIntPoint posDirection = FIntPoint(FMath::Clamp(-cellInfo.GridPos.X, -1, 1),FMath::Clamp(cellInfo.GridPos.Y, -1, 1));
			FVector rayStartPos = GetActorLocation() + (FVector(cellInfo.GridPos.X, cellInfo.GridPos.Y, 1) * 100.0f);
			FVector rayEndPos = rayStartPos + FVector(0.0f, 0.0f, -500.0f);
			FHitResult hitResult;
			FCollisionQueryParams params;
			// checks if already checked
			bool playerFoundOnThatCell = searchedLoc.Contains(rayStartPos);
			if (!playerFoundOnThatCell)
			{
				// gets grid cell
				GetWorld()->LineTraceSingleByChannel(hitResult, rayStartPos, rayEndPos, ECC_GameTraceChannel1, params);
				if (hitResult.bBlockingHit)
				{
					if (ABattleGridCell* cellObject = Cast<ABattleGridCell>(hitResult.GetActor()))
					{
						if (cellObject->CanMoveOnTo())
						{
							// if not in player direction
							if (posDirection != direction)
							{
								offTargetCell.Add(cellObject);
							}
							else
							{
								importantCells.Add(cellObject);
							}
						}
					}
				}
				// tells code this position has be checked
				searchedLoc.Add(rayStartPos);
			}
			// for every follow up
			for (const auto& followUpDirInfo : cellInfo.DirectionModifier)
			{
				FVector nextDir = FVector(0, 0, 0);
				// gets direction for follow-up cells
				switch (followUpDirInfo.Direction)
				{
				case ECompassDirection::N:
					nextDir = FVector(0, 100, 0);
					direction = FIntPoint(0, 1);
					UE_LOG(LogTemp, Warning, TEXT("North"));
					break;
				case ECompassDirection::NE:
					nextDir = FVector(100, 100, 0);
					direction = FIntPoint(1, 1);
					UE_LOG(LogTemp, Warning, TEXT("North East"));
					break;
				case ECompassDirection::E:
					nextDir = FVector(100, 0, 0);
					direction = FIntPoint(1, 0);
					UE_LOG(LogTemp, Warning, TEXT("E"));
					break;
				case ECompassDirection::SE:
					nextDir = FVector(100, -100, 0);
					direction = FIntPoint(1, -1);
					UE_LOG(LogTemp, Warning, TEXT("SE"));
					break;
				case ECompassDirection::S:
					nextDir = FVector(0, -100, 0);
					direction = FIntPoint(0, -1);
					UE_LOG(LogTemp, Warning, TEXT("S"));
					break;
				case ECompassDirection::SW:
					nextDir = FVector(-100, -100, 0);
					direction = FIntPoint(-1, -1);
					UE_LOG(LogTemp, Warning, TEXT("SW"));
					break;
				case ECompassDirection::W:
					nextDir = FVector(-100, 0, 0);
					direction = FIntPoint(-1, 0);
					UE_LOG(LogTemp, Warning, TEXT("W"));
					break;
				case ECompassDirection::NW:
					nextDir = FVector(-100, 100, 0);
					direction = FIntPoint(-1, 1);
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
					playerFoundOnThatCell = searchedLoc.Contains(prevPos);
					if (!playerFoundOnThatCell)
					{
						GetWorld()->LineTraceSingleByChannel(hitResult, prevPos, rayEndPos, ECC_GameTraceChannel1, params);
						if (hitResult.bBlockingHit)
						{
							if (ABattleGridCell* followUpCellObject = Cast<ABattleGridCell>(hitResult.GetActor()))
							{
								if (followUpCellObject->CanMoveOnTo())
								{
									if (posDirection != direction)
									{
										offTargetCell.Add(followUpCellObject);
									}
									else
									{
										importantCells.Add(followUpCellObject);
									}
								}
							}
						}
						searchedLoc.Add(prevPos);
					}
				}
			}
		}
	}
	
	if (importantCells.Num() == 0 && offTargetCell.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("This enemy doesn't have a move action :c"));
		OnActionDoneEvent.Broadcast();
		return;
	}
	
	int32 randomInt = FMath::RandRange(1, 100);
	if (randomInt < 25)
	{
		if (offTargetCell.Num() > 0)
			SetRandomTargetInList(offTargetCell);
		else
			SetRandomTargetInList(importantCells);
	}
	else
	{
		if (importantCells.Num() > 0)
			SetRandomTargetInList(importantCells);
		else
			SetRandomTargetInList(offTargetCell);
	}
	
	direction = FIntPoint(FMath::Clamp( TargetPosition.X - GetActorLocation().X, -1, 1),FMath::Clamp(TargetPosition.Y - GetActorLocation().Y, -1, 1));
	float distance = FVector::Dist(GetActorLocation(), TargetPosition);
	MoveTimer = 0;
	DesiredMoveDuration = distance/Speed;
	TimerManagerRef->ClearTimer(MoveTimerHandle);
	TimerManagerRef->SetTimer(MoveTimerHandle, this, &ABattleEnemyEntity::OnMove, 0.01f, true);
}

void ABattleEnemyEntity::OnMove()
{
	MoveTimer += 0.01f;
	FVector playerPos = GetActorLocation();
	float distance = FVector::Dist(playerPos, TargetPosition);
	
	if (MoveTimer > DesiredMoveDuration || distance < 1)
	{
		TimerManagerRef->ClearTimer(MoveTimerHandle);
		SetActorLocation(TargetPosition);
		CurrentCell->EvictEntity();
		TargetCell->HouseEntity(this);
		CurrentCell = TargetCell;
		GridPos = CurrentCell->GetGridPos();
		OnActionDoneEvent.Broadcast();
	}
	else
	{
		SetActorLocation(FMath::Lerp(playerPos, TargetPosition, MoveTimer/DesiredMoveDuration));
	}
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
void ABattleEnemyEntity::SetRandomTargetInList(TArray<ABattleGridCell*> list)
{
	int32 randomInt = FMath::RandRange(0, list.Num() - 1);
	TargetCell = list[randomInt];
	TargetPosition = list[randomInt]->GetActorLocation();
	TargetPosition.Z = GetActorLocation().Z;
}
