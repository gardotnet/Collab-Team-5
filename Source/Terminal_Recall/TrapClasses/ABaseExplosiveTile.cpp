#include "ABaseExplosiveTile.h"
#include "Engine/DamageEvents.h"

ABaseExplosiveTile::ABaseExplosiveTile()
{
	ExplosionDamage = 50.0f;
	ExplosionRadius = 200.0f;
}

void ABaseExplosiveTile::Explode_Implementation()
{
	// Implement AoE damage logic, vfx, sfx, and destroy the object
}
