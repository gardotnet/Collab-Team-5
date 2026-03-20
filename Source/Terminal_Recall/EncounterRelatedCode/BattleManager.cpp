#include "BattleManager.h"

#include "Terminal_Recall/EntityClasses/BattleEnemyEntity.h"
#include "Terminal_Recall/EntityClasses/BattleEntity.h"
#include "Terminal_Recall/EntityClasses/BattlePlayerEntity.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/Controllers/PlayerBattleController.h"
#include "Terminal_Recall/GridCellClasses/BattleGridCell.h"
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
	// allow player to select entity
	SelectMode = true;
	ABattleEntity* entityRef = selectedButton->Entity;
	// allow player to pick cells
	PlayerBattleController->bEnableMouseOverEvents = true;
	// Can put in array and link after
	PlayerBattleController->BattleChoiceUIWidget->OnCloseButtonClicked();
	FGridRangeExport* gridRange = entityRef->GetActionInfo(selectedButton->ActionType, selectedButton->IndexInOwningEntityArray);
	//TArray<ABattleGridCell*> cellsAffected;
	FIntPoint entityPos = entityRef->GetPos();
	// for each cell required for the action
	for (auto cellInfo : gridRange->Cells)
	{
		// find the action
		if (TObjectPtr<ABattleGridCell>* cell = GridMap.Find(entityPos + cellInfo.GridPos))
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
				UE_LOG(LogTemp, Warning, TEXT("We got a modifier"))
				FIntPoint prevPos = entityPos + cellInfo.GridPos;
				// until it cant continue in that direction or maximum range reached
				int32 totalFollowUp = 0;
				// get follow-up cell
				while (true)
				{
					UE_LOG(LogTemp, Warning, TEXT("Checking mag"))
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
	// if for player then enable the ability to do move and attack when cell is clicked
	/*if (selectedButton->Entity == PlayerRef)
	{
		
	}*/
}
void ABattleManager::OnCellSelected(ABattleEntity* target, EActionType type, bool mainCell)
{
	// so attack follow-ups do not trigger this
	if (mainCell || type != EActionType::Attack)
	{
		// stops player raycast as no longer needed for now
		PlayerBattleController->bEnableMouseOverEvents = false;
		// remove selection material for every cell
		CellSelectedEvent.Broadcast();
		// wipes delegates for next choice
		CellSelectedEvent.Clear();
		CallGroupHoverEvent.Clear();
		CallGroupHoverEndEvent.Clear();
		CallGroupAttackEvent.Clear();
	}
	// if there is a target to attack
	if (target == nullptr)
	{
		if (mainCell)
			NextEntityTurn();
		return;
	}
	target->CustomTakeDamage(1.0f);
	if (mainCell)
		NextEntityTurn();
}
void ABattleManager::NextEntityTurn()
{
	UE_LOG(LogTemp, Warning, TEXT("Next turn"));
	if (!PlayerRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Lost"));
		return;
	}
	// LAMBDA is fun
	// safely remove all invalid ptr from array
	EnemyArray.RemoveAll([](const TObjectPtr<ABattleEnemyEntity>& Enemy) 
	{
		return !IsValid(Enemy); 
	});
	if (EnemyArray.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Won"));
		return;
	}
	PlayerRef->OnTurnGo();
	// gets next entity's turn
	/*EntityIndex++;
	if (EntityIndex == EnemyArray.Num())
	{
		EntityIndex = -1;
		SelectMode = false;
		PlayerRef->OnTurnGo();
	}
	else
	{
		EnemyArray[EntityIndex]->OnTurnGo();
	}*/
}
void ABattleManager::BeginPlay()
{
	Super::BeginPlay();
	PlayerBattleController = Cast<APlayerBattleController>(GetWorld()->GetFirstPlayerController());
}

