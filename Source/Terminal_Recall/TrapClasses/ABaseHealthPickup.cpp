#include "ABaseHealthPickup.h"

ABaseHealthPickup::ABaseHealthPickup()
{
	HealAmount = 50.0f;
}

void ABaseHealthPickup::OnUnitArrive_Implementation()
{
	Super::OnUnitArrive_Implementation();
	
	// Can call HealPlayer in Blueprint when unit arrives
}

void ABaseHealthPickup::HealPlayer_Implementation(AActor* PlayerActor)
{
	// Blueprint handles logic to heal the player
}
