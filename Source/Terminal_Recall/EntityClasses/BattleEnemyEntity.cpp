#include "BattleEnemyEntity.h"

ABattleEnemyEntity::ABattleEnemyEntity()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABattleEnemyEntity::OnTurnGo()
{
	OnMoveDoneEvent.Broadcast();
	// Decrement all cooldowns by 1
	// AI code here to pick actions
}


