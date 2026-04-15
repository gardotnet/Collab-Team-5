#include "BaseTrapLatest.h"

ABaseTrapLatest::ABaseTrapLatest()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseTrapLatest::OnActivate_Implementation()
{
	Super::OnActivate_Implementation();
	if (bIsTriggered)
		return;
	bIsTriggered = true;
	OnActivate();
	if (!bIsPersistent)
		Destroy();
}

void ABaseTrapLatest::ResetTrap()
{
	bIsTriggered = false;
}
