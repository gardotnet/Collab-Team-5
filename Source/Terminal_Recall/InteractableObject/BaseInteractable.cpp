#include "BaseInteractable.h"

#include "Terminal_Recall/EntityClasses/BattleEntity.h"


ABaseInteractable::ABaseInteractable()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseInteractable::OnLinked_Implementation()
{
}

void ABaseInteractable::OnActivate_Implementation(ABattleEntity* entity)
{
	OnActivate(entity);
	if (!bIsPersistent)
		RemoveFromGame();
}

void ABaseInteractable::RemoveFromGame()
{
}
