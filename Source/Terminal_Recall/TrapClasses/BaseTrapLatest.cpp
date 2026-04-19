#include "BaseTrapLatest.h"

#include "Terminal_Recall/GridCellClasses/BattleGridCell.h"

ABaseTrapLatest::ABaseTrapLatest()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseTrapLatest::OnActivate_Implementation(ABattleEntity* entity)
{
	if (bIsTriggered)
		return;
	bIsTriggered = true;
	OnActivate(entity);
	if (!bIsPersistent)
		Destroy();
}

void ABaseTrapLatest::ResetTrap()
{
	bIsTriggered = false;
}

void ABaseTrapLatest::RemoveFromGame()
{
	ParentGridCell->InteractableRef = nullptr;
	Destroy();
}
