#include "ABaseTrapTiles.h"

ABaseTrapTiles::ABaseTrapTiles()
{
	DamageAmount = 20.0f;
	TurnsToRestrict = 1;
}

void ABaseTrapTiles::OnUnitArrive_Implementation()
{
	Super::OnUnitArrive_Implementation();
	
	// Implementation to apply damage and restrict movement
}

void ABaseTrapTiles::ApplyDamageAndRestriction_Implementation(AActor* TargetActor)
{
	// Logic for damage and movement restriction
}

void ABaseTrapTiles::ResetTrap_Implementation()
{
	// Logic to reset the trap and allow movement again
}

void ABaseTrapTiles::ActivateTrapFromAttack_Implementation()
{
	// Trigger trap effects if an attack reaches the tile
}
