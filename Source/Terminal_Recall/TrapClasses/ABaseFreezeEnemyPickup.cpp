#include "ABaseFreezeEnemyPickup.h"

ABaseFreezeEnemyPickup::ABaseFreezeEnemyPickup()
{
}

void ABaseFreezeEnemyPickup::OnUnitArrive_Implementation()
{
	Super::OnUnitArrive_Implementation();
	
	FreezeEnemies();
}

void ABaseFreezeEnemyPickup::FreezeEnemies_Implementation()
{
	// Blueprint will handle freezing all enemies and giving player an extra turn
}
