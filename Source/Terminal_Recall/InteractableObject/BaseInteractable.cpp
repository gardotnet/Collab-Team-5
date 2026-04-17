#include "BaseInteractable.h"

#include "Components/CapsuleComponent.h"
#include "Terminal_Recall/GridCellClasses/BattleGridCell.h"


ABaseInteractable::ABaseInteractable()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RootComponent"));
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TheMesh"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
}

void ABaseInteractable::OnLinked_Implementation()
{
}

void ABaseInteractable::OnActivate_Implementation(ABattleEntity* entity)
{
	if (bIsTriggered)
		return;
	if (Cooldown > 0)
	{
		Cooldown--;
		if (Cooldown == 0)
			ResetInteractable_Implementation();
		return;
	}
	bIsTriggered = true;
	Cooldown = CooldownTime;
	OnActivate(entity);
	if (!bIsPersistent)
		Destroy();
}

void ABaseInteractable::ResetInteractable_Implementation()
{
	bIsTriggered = false;
	ResetInteractable();
}

void ABaseInteractable::RemoveFromGame()
{
	ParentGridCell->InteractableRef = nullptr;
	Destroy();
}
