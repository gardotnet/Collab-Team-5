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
	PlayerBattleController->bEnableMouseOverEvents = true;
	if (selectedButton->Entity == PlayerRef)
	{
		// Can put in array and link after
		UE_LOG(LogTemp, Display, TEXT("%s"), *selectedButton->ActionName);
		PlayerBattleController->BattleChoiceUIWidget->OnCloseButtonClicked();
		FGridRangeExport* gridRange = PlayerRef->GetActionInfo(selectedButton->ActionType, selectedButton->IndexInOwningEntityArray);
		FIntPoint playerPos = PlayerRef->GetPos();
		
		for (auto cellPos : gridRange->Cells)
		{
			if (TObjectPtr<ABattleGridCell>* cell = GridMap.Find(playerPos + cellPos.GridPos))
			{
				 if (!(*cell)->EnableCell(selectedButton->ActionType, PlayerRef))
					continue;
				(*cell)->OnClickedCellEvent.AddDynamic(this, &ABattleManager::ABattleManager::OnCellSelected);
				CellSelectedEvent.AddDynamic(*cell, &ABattleGridCell::DisableCell);
				for (auto directionFollowUp : cellPos.DirectionModifier)
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
					FIntPoint prevPos = playerPos + cellPos.GridPos;
					while (true)
					{
						prevPos += nextDir;
						if (TObjectPtr<ABattleGridCell>* followUpCell = GridMap.Find(prevPos))
						{
							if (!(*followUpCell)->EnableCell(selectedButton->ActionType, PlayerRef))
								break;
							(*followUpCell)->OnClickedCellEvent.AddDynamic(this, &ABattleManager::ABattleManager::OnCellSelected);
							CellSelectedEvent.AddDynamic(*followUpCell, &ABattleGridCell::DisableCell);
						}
					}
				}
			}
		}
		/*for (int x = -1; x < 2; x++ )
		{
			for (int y = -1; y < 2; y++)
			{
				if (x == 0 && y == 0)
					continue;
				
				if (TObjectPtr<ABattleGridCell>* cell = GridMap.Find(playerPos + FIntPoint(x, y)))
				{
					(*cell)->EnableCell(selectedButton->ActionType, PlayerRef);
					(*cell)->OnClickedCellEvent.AddDynamic(this, &ABattleManager::ABattleManager::OnCellSelected);
					CellSelectedEvent.AddDynamic(*cell, &ABattleGridCell::DisableCell);
				}
			}
		}*/
	}
}

void ABattleManager::OnCellSelected(ABattleEntity* target)
{
	PlayerBattleController->bEnableMouseOverEvents = false;
	CellSelectedEvent.Broadcast();
	CellSelectedEvent.Clear();
	if (target == nullptr)
		return;
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

