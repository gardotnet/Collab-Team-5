#include "BattleManager.h"

#include "Terminal_Recall/EntityClasses/BattleEnemyEntity.h"
#include "Terminal_Recall/EntityClasses/BattleEntity.h"
#include "Terminal_Recall/EntityClasses/BattlePlayerEntity.h"
#include "Terminal_Recall/GridCellClasses/BattleGridCell.h"

ABattleManager::ABattleManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABattleManager::SetUpBattle(TObjectPtr<ABattlePlayerEntity> player,
	TArray<TObjectPtr<ABattleEnemyEntity>> enemies, TArray<FGridRowCell>* grid)
{
	PlayerRef = player;
	player->OnTurnDoneEvent.AddDynamic(this, &ABattleManager::NextEntityTurn);
	EnemyArray = enemies;
	// for each enemy bound an event
	for (auto enemy : enemies)
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

void ABattleManager::ActionChoiceSelected(EBattleCellState attackOrMove, EActionType ActionType)
{
	// [NOTE] Based on action chosen and if it is an attack, highlight corresponding cells.
	//GridArray[0][0]->EnableCell(attackOrMove);
	// [NOTE] if picking for player char 
	if (CurrentEntityChosen == PlayerRef)
	{
		// set it up so it call player's event (e.g. move or attack) when the cell is clicked
		//GridArray[0][0]->AddActionCauser(PlayerRef);
	}
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

