#include "BattleManager.h"

#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/EntityClasses/BattleEnemyEntity.h"
#include "Terminal_Recall/EntityClasses/BattleEntity.h"
#include "Terminal_Recall/EntityClasses/BattlePlayerEntity.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/DefaultInstance.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/Controllers/PlayerBattleController.h"
#include "Terminal_Recall/GridCellClasses/BattleGridCell.h"
#include "Terminal_Recall/InteractableObject/BaseInteractable.h"
#include "Terminal_Recall/StuffForUI/ActionButtonUI.h"
#include "Terminal_Recall/StuffForUI/BattleChoiceSelectUI.h"

ABattleManager::ABattleManager()
{
	PrimaryActorTick.bCanEverTick = false;
}
void ABattleManager::OnEntitySelected(ABattleEntity* entity)
{
	// if not deciding which cell to select
	if (SelectMode)
		return;
	entity->ShowChoices();
}
void ABattleManager::ActionChoiceSelected(UActionButtonUI* selectedButton)
{
	PlayerRef->SelectedActionID = selectedButton->IndexInOwningEntityArray;
	// allow player to select entity
	SelectMode = true;
	ABattleEntity* entityRef = selectedButton->Entity;
	// allow player to pick cells
	PlayerBattleController->bEnableMouseOverEvents = true;
	// Can put in array and link after
	PlayerBattleController->HideUI();
	FGridRangeExport* gridRange = entityRef->GetActionInfo(selectedButton->ActionType, selectedButton->IndexInOwningEntityArray);
	FIntPoint entityPos = entityRef->GetPos();
	// for each cell required for the action
	for (auto cellInfo : gridRange->Cells)
	{
		FIntPoint attackPos = entityPos + cellInfo.GridPos;
		// find the action
		if (TObjectPtr<ABattleGridCell>* cell = GridMap.Find(attackPos))
		{
			// if it can't be an option (e.g. cell is a wall)
			if (!(*cell)->EnableCell(selectedButton->ActionType, entityRef, cellInfo.GroupID))
				continue;
			// linking events
			CellSelectedEvent.AddDynamic(*cell, &ABattleGridCell::DisableCell);
			if (cellInfo.GroupID != -1)
			{
				CallGroupHoverEvent.AddDynamic(*cell, &ABattleGridCell::ForceHover);
				CallGroupHoverEndEvent.AddDynamic(*cell, &ABattleGridCell::ForceEndHover);
				CallGroupAttackEvent.AddDynamic(*cell, &ABattleGridCell::ForceAttack);
			}
			// for each modifier
			for (auto directionFollowUp : cellInfo.DirectionModifier)
			{
				FIntPoint nextDir = FIntPoint(0, 0);
				// gets direction for follow-up cells
				switch (directionFollowUp.Direction)
				{
					case ECompassDirection::N:
						nextDir = FIntPoint(0, 1);
					break;
					case ECompassDirection::NE:
						nextDir = FIntPoint(1, 1);
					break;
					case ECompassDirection::E:
						nextDir = FIntPoint(1, 0);
					break;
					case ECompassDirection::SE:
						nextDir = FIntPoint(1, -1);
					break;
					case ECompassDirection::S:
						nextDir = FIntPoint(0, -1);
					break;
					case ECompassDirection::SW:
						nextDir = FIntPoint(-1, -1);
					break;
					case ECompassDirection::W:
						nextDir = FIntPoint(-1, 0);
					break;
					case ECompassDirection::NW:
						nextDir = FIntPoint(-1, 1);
					break;
					default:
					break;
				}
				FIntPoint prevPos = attackPos;
				// until it cant continue in that direction or maximum range reached
				int32 totalFollowUp = 0;
				// get follow-up cell
				while (true)
				{
					// if it reached its range limit
					if (directionFollowUp.Magnitude != 0 && totalFollowUp >= directionFollowUp.Magnitude)
						break;
					prevPos += nextDir;
					if (TObjectPtr<ABattleGridCell>* followUpCell = GridMap.Find(prevPos))
					{
						if (!(*followUpCell)->EnableCell(selectedButton->ActionType, entityRef, cellInfo.GroupID))
							break;
						CellSelectedEvent.AddDynamic(*followUpCell, &ABattleGridCell::DisableCell);
						if (cellInfo.GroupID != -1)
						{
							CallGroupHoverEvent.AddDynamic(*followUpCell, &ABattleGridCell::ForceHover);
							CallGroupHoverEndEvent.AddDynamic(*followUpCell, &ABattleGridCell::ForceEndHover);
							CallGroupAttackEvent.AddDynamic(*followUpCell, &ABattleGridCell::ForceAttack);
						}
						totalFollowUp++;
					}
					// if unable to find, break
					else
						break;
				}
			}
		}
	}
}

void ABattleManager::ActionSelected(int hotbarSlot)
{
	ClearPrediction();
	PlayerRef->SelectedActionID = hotbarSlot;
	UE_LOG(LogTemp, Error, TEXT("%d"), PlayerRef->SelectedActionID);
	// allow player to select entity
	SelectMode = true;
	// allow player to pick cells
	PlayerBattleController->bEnableMouseOverEvents = true;
	// Can put in array and link after
	//PlayerBattleController->HideUI();
	FGridRangeExport* gridRange = PlayerRef->GetActionInfo(PlayerActionType, hotbarSlot);
	FIntPoint entityPos = PlayerRef->GetPos();
	// for each cell required for the action
	for (auto cellInfo : gridRange->Cells)
	{
		FIntPoint attackPos = entityPos + cellInfo.GridPos;
		// find the action
		if (TObjectPtr<ABattleGridCell>* cell = GridMap.Find(attackPos))
		{
			// if it can't be an option (e.g. cell is a wall)
			if (!(*cell)->EnableCell(PlayerActionType, PlayerRef, cellInfo.GroupID))
				continue;
			// linking events
			CellSelectedEvent.AddDynamic(*cell, &ABattleGridCell::DisableCell);
			if (cellInfo.GroupID != -1)
			{
				CallGroupHoverEvent.AddDynamic(*cell, &ABattleGridCell::ForceHover);
				CallGroupHoverEndEvent.AddDynamic(*cell, &ABattleGridCell::ForceEndHover);
				CallGroupAttackEvent.AddDynamic(*cell, &ABattleGridCell::ForceAttack);
			}
			// for each modifier
			for (auto directionFollowUp : cellInfo.DirectionModifier)
			{
				FIntPoint nextDir = FIntPoint(0, 0);
				// gets direction for follow-up cells
				switch (directionFollowUp.Direction)
				{
					case ECompassDirection::N:
						nextDir = FIntPoint(0, 1);
					break;
					case ECompassDirection::NE:
						nextDir = FIntPoint(1, 1);
					break;
					case ECompassDirection::E:
						nextDir = FIntPoint(1, 0);
					break;
					case ECompassDirection::SE:
						nextDir = FIntPoint(1, -1);
					break;
					case ECompassDirection::S:
						nextDir = FIntPoint(0, -1);
					break;
					case ECompassDirection::SW:
						nextDir = FIntPoint(-1, -1);
					break;
					case ECompassDirection::W:
						nextDir = FIntPoint(-1, 0);
					break;
					case ECompassDirection::NW:
						nextDir = FIntPoint(-1, 1);
					break;
					default:
					break;
				}
				FIntPoint prevPos = attackPos;
				// until it cant continue in that direction or maximum range reached
				int32 totalFollowUp = 0;
				// get follow-up cell
				while (true)
				{
					// if it reached its range limit
					if (directionFollowUp.Magnitude != 0 && totalFollowUp >= directionFollowUp.Magnitude)
						break;
					prevPos += nextDir;
					if (TObjectPtr<ABattleGridCell>* followUpCell = GridMap.Find(prevPos))
					{
						if (!(*followUpCell)->EnableCell(PlayerActionType, PlayerRef, cellInfo.GroupID))
							break;
						CellSelectedEvent.AddDynamic(*followUpCell, &ABattleGridCell::DisableCell);
						if (cellInfo.GroupID != -1)
						{
							CallGroupHoverEvent.AddDynamic(*followUpCell, &ABattleGridCell::ForceHover);
							CallGroupHoverEndEvent.AddDynamic(*followUpCell, &ABattleGridCell::ForceEndHover);
							CallGroupAttackEvent.AddDynamic(*followUpCell, &ABattleGridCell::ForceAttack);
						}
						totalFollowUp++;
					}
					// if unable to find, break
					else
						break;
				}
			}
		}
	}
}

void ABattleManager::ToggleMoveOrAttack()
{
	if (PlayerActionType == EActionType::Movement)
		PlayerActionType = EActionType::Attack;
	else
		PlayerActionType = EActionType::Movement;
}

void ABattleManager::OnCellSelected(ABattleEntity* target, EActionType type, bool mainAttackCell)
{
	// so attack follow-ups do not trigger this
	if (mainAttackCell || (type != EActionType::Attack && type != EActionType::EnemyAttack))
	{
		// stops player raycast as no longer needed for now
		PlayerBattleController->bEnableMouseOverEvents = false;
		ClearPrediction();
	}
	// if there is a target to attack
	if (IsValid(target) && type == EActionType::Attack)
	{
		target->CustomTakeDamage(PlayerRef->GetDamage());
	}
	
	if (mainAttackCell && !(type == EActionType::EnemyAttack || type == EActionType::EnemyMove))
	{
		PlayerRef->SelectedActionID = -1;
		UE_LOG(LogTemp, Error, TEXT("%d"), PlayerRef->SelectedActionID);
		PlayerRef->OnActionComplete(type);
		OnPlayerEndTurnEvent.Broadcast();
		NextEntityTurn();
		return;
	}
	SelectMode = false;
}

void ABattleManager::NextEntityTurn()
{
	if (!IsValid(PlayerRef))
	{
		UDefaultInstance* theInstance = Cast<UDefaultInstance>(GetGameInstance());
		theInstance->Health = theInstance->MaxHealth;
		UGameplayStatics::OpenLevel(GetWorld(), MainGameLevel);
		return;
	}
	// LAMBDA is fun
	// safely remove all invalid ptr from array
	EnemyArray.RemoveAll([](const TObjectPtr<ABattleEnemyEntity>& Enemy) 
	{
		return !IsValid(Enemy); 
	});
	if (EnemyArray.IsEmpty() && !bFreeMode)
	{
		UDefaultInstance* theInstance = Cast<UDefaultInstance>(GetGameInstance());
		theInstance->Health = PlayerRef->CurrentHealth;
		theInstance->EncounterWon();
		if (IsValid(theInstance->RobotPickUpClass))
		{
			if (TObjectPtr<ABattleGridCell>* newCell = GridMap.Find(theInstance->PickUpLoc))
			{
				for (AActor* trap : TrapList)
				{
					ABaseInteractable* convertedTrap = Cast<ABaseInteractable>(trap);
					if (IsValid(convertedTrap))
					{
						convertedTrap->RemoveFromGame();
					}
				}
				AActor* pickUp = GetWorld()->SpawnActor<AActor>(theInstance->RobotPickUpClass);
				FVector positionToSpawn = (*newCell)->GetActorLocation() + FVector(0,0,10);
				FTransform transform = FTransform(FRotator::ZeroRotator, positionToSpawn, FVector::OneVector);
				pickUp->SetActorTransform(transform);
				(*newCell)->AssignInteractable(Cast<ABaseInteractable>(pickUp));
				bFreeMode = true;
				return;
			}
		}
		UGameplayStatics::OpenLevel(GetWorld(), MainGameLevel);
		return;
	}
	// gets next entity's turn
	EntityIndex++;
	if (EntityIndex >= EnemyArray.Num())
	{
		OnNewTurnEvent.Broadcast();
		//PlayerBattleController->BattleChoiceUIWidget->AddToViewport();
		EntityIndex = -1;
		SelectMode = false;
		PlayerRef->OnTurnGo();
	}
	else
	{
		EnemyArray[EntityIndex]->OnTurnGo();
	}
}

void ABattleManager::ClearPrediction()
{
	SelectMode = false;
	PlayerBattleController->bEnableMouseOverEvents = false;
	// remove selection material for every cell
	CellSelectedEvent.Broadcast();
	// wipes delegates for next choice
	CellSelectedEvent.Clear();
	CallGroupHoverEvent.Clear();
	CallGroupHoverEndEvent.Clear();
	CallGroupAttackEvent.Clear();
}

void ABattleManager::BeginPlay()
{
	Super::BeginPlay();
	PlayerBattleController = Cast<APlayerBattleController>(GetWorld()->GetFirstPlayerController());
}
void ABattleManager::BeginDestroy()
{
	Super::BeginDestroy();
	CellSelectedEvent.Clear();
	CallGroupHoverEvent.Clear();
	CallGroupHoverEndEvent.Clear();
	CallGroupAttackEvent.Clear();
}