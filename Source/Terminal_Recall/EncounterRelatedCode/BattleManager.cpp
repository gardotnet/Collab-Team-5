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
		PlayerRef->OnTurnDoneEvent.AddDynamic(this, &ABattleManager::NextEntityTurn);
		PlayerRef->OnBattleStart(this);
	}
	// for each enemy bound an event
	for (auto enemy : EnemyArray)
	{
		enemy->OnTurnDoneEvent.AddDynamic(this, &ABattleManager::NextEntityTurn);
	}
}

void ABattleManager::OnEntitySelected(ABattleEntity* entity)
{
	if (CurrentEntityChosen)
		CurrentEntityChosen->HideChoices();
	CurrentEntityChosen = entity;
	entity->ShowChoices();
}

void ABattleManager::ActionChoiceSelected(UActionButtonUI* selectedButton)
{
	if (selectedButton->Entity == PlayerRef)
	{
		// Can put in array and link after
		UE_LOG(LogTemp, Display, TEXT("%s"), *selectedButton->ActionName);
		PlayerBattleController->BattleChoiceUIWidget->OnCloseButtonClicked();
		if (selectedButton->AttackType != EAttackActionTypes::Custom)
		{
			switch (selectedButton->AttackType)
			{
				case EAttackActionTypes::King:
					FIntPoint playerPos = PlayerRef->GetPos();
					for (int x = -1; x < 2; x++ )
					{
						for (int y = -1; y < 2; y++)
						{
							if (x == 0 && y == 0)
								continue;
							
							if (TObjectPtr<ABattleGridCell>* cell = GridMap.Find(playerPos + FIntPoint(x, y)))
							{
								(*cell)->EnableCell();
								(*cell)->OnClickedCellEvent.AddDynamic(this, &ABattleManager::ABattleManager::OnCellSelected);
								CellSelectedEvent.AddDynamic(*cell, &ABattleGridCell::DisableCell);
							}
						}
					}
				break;
				case EAttackActionTypes::Rook:
				break;
				case EAttackActionTypes::Bishop:
				break;
				case EAttackActionTypes::Knight:
				break;
				case EAttackActionTypes::Queen:
				break;
				default:
				break;
			}
		}
	}
}

void ABattleManager::OnCellSelected(ABattleGridCell* selectedCell)
{
	CellSelectedEvent.Broadcast();
	CellSelectedEvent.Clear();
	PlayerRef->SetPos(selectedCell->GetGridPos());
	PlayerRef->StartMove(selectedCell->GetActorLocation());
}

void ABattleManager::NextEntityTurn()
{
	EntityIndex++;
	if (EntityIndex == EnemyArray.Num())
	{
		EntityIndex = -1;
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

