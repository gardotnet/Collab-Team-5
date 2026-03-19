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
	PrimaryActorTick.bCanEverTick = true;
}

void ABattleManager::SetUpBattle()
{
	if (PlayerRef)
	{
		PlayerRef->OnMoveDoneEvent.AddDynamic(this, &ABattleManager::NextEntityTurn);
	}
	// for each enemy bound an event
	for (auto enemy : EnemyArray)
	{
		enemy->OnMoveDoneEvent.AddDynamic(this, &ABattleManager::NextEntityTurn);
	}
}

void ABattleManager::OnEntitySelected(ABattleEntity* entity)
{
	if (SelectMode)
		return;
	entity->ShowChoices();
}

void ABattleManager::ActionChoiceSelected(UActionButtonUI* selectedButton)
{
	SelectMode = true;
	if (selectedButton->Entity == PlayerRef)
	{
		PlayerBattleController->bEnableMouseOverEvents = true;
		// Can put in array and link after
		UE_LOG(LogTemp, Display, TEXT("%s"), *selectedButton->ActionName);
		PlayerBattleController->BattleChoiceUIWidget->OnCloseButtonClicked();
		FGridRangeExport* gridRange = PlayerRef->GetActionInfo(selectedButton->ActionType, selectedButton->IndexInOwningEntityArray);
		FIntPoint playerPos = PlayerRef->GetPos();
		
		for (auto cellInfo : gridRange->Cells)
		{
			if (TObjectPtr<ABattleGridCell>* cell = GridMap.Find(playerPos + cellInfo.GridPos))
			{
				 if (!(*cell)->EnableCell(selectedButton->ActionType, PlayerRef, cellInfo.GroupID))
					continue;
				UE_LOG(LogTemp, Warning, TEXT("Group ID: %d"), cellInfo.GroupID);
				CellSelectedEvent.AddDynamic(*cell, &ABattleGridCell::DisableCell);
				if (cellInfo.GroupID != -1)
				{
					CallGroupHoverEvent.AddDynamic(*cell, &ABattleGridCell::ForceHover);
					CallGroupHoverEndEvent.AddDynamic(*cell, &ABattleGridCell::ForceEndHover);
					CallGroupAttackEvent.AddDynamic(*cell, &ABattleGridCell::ForceAttack);
				}
				for (auto directionFollowUp : cellInfo.DirectionModifier)
				{
					FIntPoint nextDir = FIntPoint(0, 0);
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
					FIntPoint prevPos = playerPos + cellInfo.GridPos;
					while (true)
					{
						prevPos += nextDir;
						if (TObjectPtr<ABattleGridCell>* followUpCell = GridMap.Find(prevPos))
						{
							if (!(*followUpCell)->EnableCell(selectedButton->ActionType, PlayerRef, cellInfo.GroupID))
								break;
							CellSelectedEvent.AddDynamic(*followUpCell, &ABattleGridCell::DisableCell);
							if (cellInfo.GroupID != -1)
							{
								CallGroupHoverEvent.AddDynamic(*followUpCell, &ABattleGridCell::ForceHover);
								CallGroupHoverEndEvent.AddDynamic(*followUpCell, &ABattleGridCell::ForceEndHover);
								CallGroupAttackEvent.AddDynamic(*followUpCell, &ABattleGridCell::ForceAttack);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		// Do enemy predict
	}
}

void ABattleManager::OnCellSelected(ABattleEntity* target)
{
	PlayerBattleController->bEnableMouseOverEvents = false;
	CellSelectedEvent.Broadcast();
	
	CellSelectedEvent.Clear();
	CallGroupHoverEvent.Clear();
	CallGroupHoverEndEvent.Clear();
	CallGroupAttackEvent.Clear();
	
	if (target == nullptr)
	{
		NextEntityTurn();
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Attacking"));
	
	target->CustomTakeDamage(1.0f);
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
		
	EntityIndex++;
	if (EntityIndex == EnemyArray.Num())
	{
		EntityIndex = -1;
		SelectMode = false;
		PlayerRef->OnTurnGo();
	}
	else
	{
		EnemyArray[EntityIndex]->OnTurnGo();
	}
}

void ABattleManager::BeginPlay()
{
	Super::BeginPlay();
	PlayerBattleController = Cast<APlayerBattleController>(GetWorld()->GetFirstPlayerController());
}

