#include "BattlePlayerEntity.h"

ABattlePlayerEntity::ABattlePlayerEntity()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABattlePlayerEntity::OnBattleStart()
{
	Super::OnBattleStart();
	// transfer stuff from overworld 
}
