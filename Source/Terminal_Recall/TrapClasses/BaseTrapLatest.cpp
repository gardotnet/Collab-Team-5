#include "BaseTrapLatest.h"

#include "Terminal_Recall/GridCellClasses/BattleGridCell.h"

ABaseTrapLatest::ABaseTrapLatest()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseTrapLatest::OnActivate_Implementation(ABattleEntity* entity)
{
	Super::OnActivate_Implementation(entity);
}

void ABaseTrapLatest::RemoveFromGame()
{
	ParentGridCell->InteractableRef = nullptr;
	Destroy();
}
