#include "ABaseQuicksandTile.h"

ABaseQuicksandTile::ABaseQuicksandTile()
{
}

void ABaseQuicksandTile::OnUnitArrive_Implementation()
{
	Super::OnUnitArrive_Implementation();
	// Logic to apply slowdown will be handled in BP by calling ApplySlowdown
}

void ABaseQuicksandTile::OnUnitLeave_Implementation(AActor* LeavingActor)
{
	// Logic to remove slowdown will be handled in BP by calling RemoveSlowdown
}

void ABaseQuicksandTile::ApplySlowdown_Implementation(AActor* TargetActor)
{
	// Restrict to King movement for player or 1 tile movement for enemies
}

void ABaseQuicksandTile::RemoveSlowdown_Implementation(AActor* TargetActor)
{
	// Restore normal movement
}
