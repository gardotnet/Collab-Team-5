#include "BaseTrapLatest.h"

#include "Terminal_Recall/GridCellClasses/BattleGridCell.h"

ABaseTrapLatest::ABaseTrapLatest()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseTrapLatest::OnActivate_Implementation(ABattleEntity* entity)
{
	Super::OnActivate_Implementation(entity);
	if (bIsTriggered)
		return;
	bIsTriggered = true;
	OnActivate();
	if (!bIsPersistent)
		Destroy();
}

void ABaseTrapLatest::ResetTrap()
{
	bIsTriggered = false;
}
