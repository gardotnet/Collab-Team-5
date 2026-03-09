#include "BattleEnemyEntity.h"

ABattleEnemyEntity::ABattleEnemyEntity()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABattleEnemyEntity::OnTurnGo()
{
	// Decrement all cooldowns by 1
	// AI code here to pick actions
}


