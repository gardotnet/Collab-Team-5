#include "ABaseCrackedTiles.h"

ABaseCrackedTiles::ABaseCrackedTiles()
{
	MaxStepCount = 2;
	CurrentStepCount = 0;
}

void ABaseCrackedTiles::OnUnitArrive_Implementation()
{
	Super::OnUnitArrive_Implementation();
	
	CurrentStepCount++;
	if (CurrentStepCount >= MaxStepCount)
	{
		BreakTile();
	}
}

void ABaseCrackedTiles::BreakTile_Implementation()
{
	// Logic to make the tile inaccessible (e.g., creating an empty slot on the grid)
}
