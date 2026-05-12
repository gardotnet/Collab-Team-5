#include "BattleEnemyEntity.h"

#include "BattlePlayerEntity.h"
#include "Terminal_Recall/EncounterRelatedCode/BattleManager.h"
#include "Terminal_Recall/GridCellClasses/BattleGridCell.h"
#include "Terminal_Recall/InteractableObject/BaseInteractable.h"
#include "Terminal_Recall/StructsAndEnums/ForDataTable/EnemyInfoStruct.h"

ABattleEnemyEntity::ABattleEnemyEntity()
{
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent->OnBeginCursorOver.AddDynamic(this, &ABattleEnemyEntity::OnHoverOver);
	MeshComponent->OnEndCursorOver.AddDynamic(this, &ABattleEnemyEntity::OnHoverEnd);
}
void ABattleEnemyEntity::OnTurnGo()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemies turn"))
	TArray<int32> attacksThatCanHit;
	TMap<FVector, bool> searchedLoc;
	PlayerRef = nullptr;
	// checks if player can be hit
	for (int i = 0; i < 5; i++)
	{
		if (ActionList[i].Cooldown != 0)
			continue;
		if (ActionList[i].AttackAction.ActionRange.Cells.Num() == 0)
		{
			attacksThatCanHit.Add(i);
			continue;
		}
		for (const auto& cellInfo : ActionList[i].AttackAction.ActionRange.Cells)
		{
			FVector rayStartPos = GetActorLocation() + (FVector(cellInfo.GridPos.X, cellInfo.GridPos.Y, 1) * 100.0f);
			FVector rayEndPos = rayStartPos + FVector(0.0f, 0.0f, -500.0f);
			FHitResult hitResult;
			FCollisionQueryParams params;
			if (bool* playerFoundOnThatCell = searchedLoc.Find(rayStartPos))
			{
				if (*playerFoundOnThatCell)
					if (!attacksThatCanHit.Contains(i))
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
					break;
				case ECompassDirection::NE:
					nextDir = FVector(100, 100, 0);
					break;
				case ECompassDirection::E:
					nextDir = FVector(100, 0, 0);
					break;
				case ECompassDirection::SE:
					nextDir = FVector(100, -100, 0);
					break;
				case ECompassDirection::S:
					nextDir = FVector(0, -100, 0);
					break;
				case ECompassDirection::SW:
					nextDir = FVector(-100, -100, 0);
					break;
				case ECompassDirection::W:
					nextDir = FVector(-100, 0, 0);
					break;
				case ECompassDirection::NW:
					nextDir = FVector(-100, 100, 0);
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
							if (!attacksThatCanHit.Contains(i))
								attacksThatCanHit.Add(i);
						break;
					}
					GetWorld()->LineTraceSingleByChannel(hitResult, prevPos, rayEndPos, ECC_GameTraceChannel2, params);
					if (hitResult.bBlockingHit)
					{
						PlayerRef = Cast<ABattlePlayerEntity>(hitResult.GetActor());
						if (IsValid(PlayerRef))
						{
							if (!attacksThatCanHit.Contains(i))
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
			SelectedActionID = FMath::RandRange(0, attacksThatCanHit.Num() - 1);
			UE_LOG(LogTemp, Warning, TEXT("Should have avoided my range"));
			if (ActionList[SelectedActionID].AttackAction.ActionRange.Cells.Num() == 0)
			{
				if (IsValid(ActionList[SelectedActionID].AttackAction.StatusToApply))
				{
					UE_LOG(LogTemp, Warning, TEXT("THERE's effects"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("No effects awww?"));
				}
				DelayAttack();
			}
			else
			{
				TArray<FVector> cellsDoneChecked;
				for (const auto& cellInfo : ActionList[SelectedActionID].AttackAction.ActionRange.Cells)
				{
					FVector rayStartPos = GetActorLocation() + (FVector(cellInfo.GridPos.X, cellInfo.GridPos.Y, 1) * 100.0f);
					FVector rayEndPos = rayStartPos + FVector(0.0f, 0.0f, -500.0f);
					FHitResult hitResult;
					FCollisionQueryParams params;
					if (!cellsDoneChecked.Contains(rayStartPos))
					{
						GetWorld()->LineTraceSingleByChannel(hitResult, rayStartPos, rayEndPos, ECC_GameTraceChannel1, params);
						if (hitResult.bBlockingHit)
						{
							ABattleGridCell* gridCell = Cast<ABattleGridCell>(hitResult.GetActor());
							if (IsValid(gridCell))
							{
								gridCell->ForceHighlight(this);
								HighlightedCells.Add(gridCell);
							}
						}
						cellsDoneChecked.Add(rayStartPos);
					}
					for (const auto& followUpDirInfo : cellInfo.DirectionModifier)
					{
						FVector nextDir = FVector(0, 0, 0);
						// gets direction for follow-up cells
						switch (followUpDirInfo.Direction)
						{
						case ECompassDirection::N:
							nextDir = FVector(0, 100, 0);
							break;
						case ECompassDirection::NE:
							nextDir = FVector(100, 100, 0);
							break;
						case ECompassDirection::E:
							nextDir = FVector(100, 0, 0);
							break;
						case ECompassDirection::SE:
							nextDir = FVector(100, -100, 0);
							break;
						case ECompassDirection::S:
							nextDir = FVector(0, -100, 0);
							break;
						case ECompassDirection::SW:
							nextDir = FVector(-100, -100, 0);
							break;
						case ECompassDirection::W:
							nextDir = FVector(-100, 0, 0);
							break;
						case ECompassDirection::NW:
							nextDir = FVector(-100, 100, 0);
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
							if (!cellsDoneChecked.Contains(prevPos))
							{
								GetWorld()->LineTraceSingleByChannel(hitResult, prevPos, rayEndPos, ECC_GameTraceChannel1, params);
								if (hitResult.bBlockingHit)
								{
									ABattleGridCell* followUpCells = Cast<ABattleGridCell>(hitResult.GetActor());
									if (IsValid(followUpCells))
									{
										followUpCells->ForceHighlight(this);
										HighlightedCells.Add(followUpCells);
									}
								}
								cellsDoneChecked.Add(prevPos);
							}
							totalFollowUp++;
						}
					}
				}
				FTimerHandle timerHandle;
				TimerManagerRef->SetTimer(timerHandle, this, &ABattleEnemyEntity::DelayAttack, 3.0f, false);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Moving"))
		StartMove(FVector::ZeroVector);
	}
}

void ABattleEnemyEntity::StartMove(FVector position)
{
	if (!IsValid(PlayerRef))
		PlayerRef = BattleManager->PlayerRef;
	FIntPoint playerPos = PlayerRef->GetPos();
	FIntPoint direction = FIntPoint(FMath::Clamp( playerPos.X - GridPos.X, -1, 1),FMath::Clamp(playerPos.Y - GridPos.Y, -1, 1));
	TArray<FPotentialMoveCellInfo> importantCells;
	TArray<FPotentialMoveCellInfo> offTargetCell;
	TArray<FVector> searchedLoc;
	
	for (int i = 0; i < 5; i++)
	{
		if (ActionList[i].Cooldown != 0)
			continue;
		for (const auto& cellInfo : ActionList[i].MovementAction.ActionRange.Cells)
		{
			// get pos, dir and set up raycasting
			FIntPoint posDirection = FIntPoint(FMath::Clamp(-cellInfo.GridPos.X, -1, 1),FMath::Clamp(cellInfo.GridPos.Y, -1, 1));
			FVector rayStartPos = GetActorLocation() + (FVector(cellInfo.GridPos.X, cellInfo.GridPos.Y, 1) * 100.0f);
			FVector rayEndPos = rayStartPos + FVector(0.0f, 0.0f, -500.0f);
			FHitResult hitResult;
			FCollisionQueryParams params;
			// checks if already checked
			bool alrSearched = searchedLoc.Contains(rayStartPos);
			// gets grid cell
			GetWorld()->LineTraceSingleByChannel(hitResult, rayStartPos, rayEndPos, ECC_GameTraceChannel1, params);
			if (hitResult.bBlockingHit)
			{
				if (ABattleGridCell* cellObject = Cast<ABattleGridCell>(hitResult.GetActor()))
				{
					if (alrSearched)
					{
						FPotentialMoveCellInfo* foundInfo = importantCells.FindByPredicate([cellObject](const FPotentialMoveCellInfo& Item)
						{
							return Item.Cell == cellObject;
						});
						if (foundInfo)
						{
							if (!foundInfo->RelatedActionIdInList.Contains(i))
								foundInfo->RelatedActionIdInList.Add(i);
						}
					}
					else
					{
						if (cellObject->CanMoveOnTo())
						{
							FPotentialMoveCellInfo newCellInfo;
							newCellInfo.Cell = cellObject;
							newCellInfo.RelatedActionIdInList.Add(i);
							// if not in player direction
							if (posDirection != direction)
							{
								offTargetCell.Add(newCellInfo);
							}
							else
							{
								importantCells.Add(newCellInfo);
							}
						}
					}
				}
			}
			// tells code this position has be checked
			searchedLoc.Add(rayStartPos);
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
					break;
				case ECompassDirection::NE:
					nextDir = FVector(100, 100, 0);
					direction = FIntPoint(1, 1);
					break;
				case ECompassDirection::E:
					nextDir = FVector(100, 0, 0);
					direction = FIntPoint(1, 0);
					break;
				case ECompassDirection::SE:
					nextDir = FVector(100, -100, 0);
					direction = FIntPoint(1, -1);
					break;
				case ECompassDirection::S:
					nextDir = FVector(0, -100, 0);
					direction = FIntPoint(0, -1);
					break;
				case ECompassDirection::SW:
					nextDir = FVector(-100, -100, 0);
					direction = FIntPoint(-1, -1);
					break;
				case ECompassDirection::W:
					nextDir = FVector(-100, 0, 0);
					direction = FIntPoint(-1, 0);
					break;
				case ECompassDirection::NW:
					nextDir = FVector(-100, 100, 0);
					direction = FIntPoint(-1, 1);
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
					alrSearched = searchedLoc.Contains(prevPos);
					
					GetWorld()->LineTraceSingleByChannel(hitResult, prevPos, rayEndPos, ECC_GameTraceChannel1, params);
					if (hitResult.bBlockingHit)
					{
						if (ABattleGridCell* followUpCellObject = Cast<ABattleGridCell>(hitResult.GetActor()))
						{
							if (alrSearched)
							{
								FPotentialMoveCellInfo* foundInfo = importantCells.FindByPredicate([followUpCellObject](const FPotentialMoveCellInfo& Item)
								{
									return Item.Cell == followUpCellObject;
								});
								if (foundInfo)
								{
									if (!foundInfo->RelatedActionIdInList.Contains(i))
										foundInfo->RelatedActionIdInList.Add(i);
								}
								continue;
							}
							if (followUpCellObject->CanMoveOnTo())
							{
								FPotentialMoveCellInfo newCellInfo;
								newCellInfo.Cell = followUpCellObject;
								newCellInfo.RelatedActionIdInList.Add(i);
								if (posDirection != direction)
								{
									offTargetCell.Add(newCellInfo);
								}
								else
								{
									importantCells.Add(newCellInfo);
								}
							}
						}
					}
					searchedLoc.Add(prevPos);
					totalFollowUp++;
				}
			}
		}
	}
	
	if (importantCells.Num() == 0 && offTargetCell.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("This enemy doesn't have a move action :c"));
		//OnActionDoneEvent.Broadcast();
		BattleManager->NextEntityTurn();
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
		BattleManager->NextEntityTurn();
	}
	else
	{
		SetActorLocation(FMath::Lerp(playerPos, TargetPosition, MoveTimer/DesiredMoveDuration));
	}
}

void ABattleEnemyEntity::OnBattleStart(ABattleManager* battleManager, FEnemyInfoStruct* enemyInfoStruct)
{
	BattleManager = battleManager;
	FindAndSaveActions(enemyInfoStruct->AttackIdList, enemyInfoStruct->MovementIdList);
	CurrentHealth = enemyInfoStruct->Health;
	MaxHealth = enemyInfoStruct->Health;
	DamageMultiplier = 1;
	HeathDropChance = enemyInfoStruct->HealthDropChance;
}

void ABattleEnemyEntity::CustomTakeDamage(float damage)
{
	CurrentHealth = CurrentHealth - damage;
	OnDamagedEvent.Broadcast(CurrentHealth);
	if (CurrentHealth <= 0.0f)
	{
		// tells the grid it 'WAS' on, that it is now empty
		OnChangeCell.Broadcast();
		if (IsValid(HealthPickUpClass))
		{
			int32 randomInt = FMath::RandRange(1, 100);
			if (randomInt <= HeathDropChance)
			{
				if (IsValid(CurrentCell) && CurrentCell->InteractableRef == nullptr)
				{
					FActorSpawnParameters spawnParams;
					spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					CurrentCell->AssignInteractable(GetWorld()->SpawnActor<ABaseInteractable>(HealthPickUpClass, CurrentCell->GetActorTransform(), spawnParams));
				}
			}
		}
		BattleManager->EnemyArray.Remove(this);
		Destroy();
		
		FTimerHandle EnemyDeathDelay;
		GetWorld()->GetTimerManager().SetTimer(
			EnemyDeathDelay,
			BattleManager.Get(),
			&ABattleManager::NextEntityTurn,
			0.3f,
			false
		);
		
	}
	else if (CurrentHealth > MaxHealth)
	{
		CurrentHealth = MaxHealth;
	}
}

void ABattleEnemyEntity::DelayAttack()
{
	for (const auto& cell : HighlightedCells)
	{
		cell->EndForceHighlight();
	}
	if (IsValid(ActionList[SelectedActionID].AttackAction.StatusToApply))
	{
		ABaseStatusEffect* statusToAdd = GetWorld()->SpawnActor<ABaseStatusEffect>(ActionList[SelectedActionID].AttackAction.StatusToApply);
		if (ActionList[SelectedActionID].AttackAction.bIsSelfAppliedStatus)
		{
			CurrentStatusEffects.Add(statusToAdd);
		}
		else
		{
			PlayerRef->CustomTakeDamageAndApplyStatus(ActionList[SelectedActionID].AttackAction.Damage * DamageMultiplier, statusToAdd);
		}
	}
	else
	{
		PlayerRef->CustomTakeDamage(ActionList[SelectedActionID].AttackAction.Damage * DamageMultiplier);
	}
	OnActionComplete(EActionType::Attack);
	//OnActionDoneEvent.Broadcast();
	BattleManager->NextEntityTurn();
}

void ABattleEnemyEntity::SetRandomTargetInList(const TArray<FPotentialMoveCellInfo>& list)
{
	int32 randomInt = FMath::RandRange(0, list.Num() - 1);
	TargetCell = list[randomInt].Cell;
	TargetPosition = list[randomInt].Cell->GetActorLocation();
	TargetPosition.Z = GetActorLocation().Z;
	SelectedActionID = list[randomInt].RelatedActionIdInList[0];
	if (list[randomInt].RelatedActionIdInList.Num() != 1)
	{
		int32 randomIndex = FMath::RandRange(0, list[randomInt].RelatedActionIdInList.Num() - 1);
		SelectedActionID = list[randomInt].RelatedActionIdInList[randomIndex];
	}
	OnActionComplete(EActionType::Movement);
}

void ABattleEnemyEntity::OnEntityClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	//PlayerBattleController->MainBattleUIWidget->SetEnemyHp(CurrentHealth);
	if (CurrentCell != nullptr)
	{
		CurrentCell->OnCellClicked(nullptr, FKey());
	}
}

void ABattleEnemyEntity::OnHoverOver(UPrimitiveComponent* TouchedComponent)
{
	if (CurrentCell != nullptr)
	{
		CurrentCell->OnHoverOver(TouchedComponent);
	}
}

void ABattleEnemyEntity::OnHoverEnd(UPrimitiveComponent* TouchedComponent)
{
	if (CurrentCell != nullptr)
	{
		CurrentCell->OnHoverEnd(TouchedComponent);
	}
}
