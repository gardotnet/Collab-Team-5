#include "DefaultInstance.h"

void UDefaultInstance::EncounterWon()
{
	VisitedCells.Add(PlayerSpawnPos);
}

void UDefaultInstance::AddVisitedCells(FVector newPos)
{
	VisitedCells.Add(newPos);
}
