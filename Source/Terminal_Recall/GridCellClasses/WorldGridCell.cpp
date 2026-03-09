#include "WorldGridCell.h"

AWorldGridCell::AWorldGridCell()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWorldGridCell::LoadLevel()
{
	// [NOTE] Will tell encounter "level loader" to load the level assign to this cell.
}

void AWorldGridCell::OnCellClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	// [NOTE] Tell player controller to target this cell
}

