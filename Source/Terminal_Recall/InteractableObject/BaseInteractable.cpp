#include "BaseInteractable.h"


ABaseInteractable::ABaseInteractable()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseInteractable::OnActivate_Implementation()
{
}
