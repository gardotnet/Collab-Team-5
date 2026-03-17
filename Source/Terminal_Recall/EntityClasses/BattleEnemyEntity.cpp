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

void ABattleEnemyEntity::OnBattleStart(ABattleManager* battleManager, float health, float damageMultiplier)
{
	Super::OnBattleStart(battleManager, health, damageMultiplier);
	// Create UI buttons and linking it to 'OnAttackSelected' or 'OnMovementSelected'
	// Also store it to show/hide the choices
}


