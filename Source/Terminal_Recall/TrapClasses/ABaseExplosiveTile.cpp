#include "ABaseExplosiveTile.h"
#include "Engine/DamageEvents.h"

ABaseExplosiveTile::ABaseExplosiveTile()
{
	ExplosionDamage = 50.0f;
	ExplosionRadius = 200.0f;
}

float ABaseExplosiveTile::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (ActualDamage > 0.0f)
	{
		Explode();
	}
	
	return ActualDamage;
}

void ABaseExplosiveTile::Explode_Implementation()
{
	// Implement AoE damage logic, vfx, sfx, and destroy the object
}
