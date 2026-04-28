#include "BaseInteractable.h"

#include "Components/CapsuleComponent.h"
#include "Terminal_Recall/EncounterRelatedCode/BattleManager.h"
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

void ABaseInteractable::ReduceCooldown()
{
	Cooldown--;
	if (Cooldown == 0)
	{
		ParentGridCell->BattleManager->OnNewTurnEvent.RemoveDynamic(this, &ABaseInteractable::ReduceCooldown);
		bIsTriggered = false;
		OnTrapResetEvent.Broadcast();
	}
}

void ABaseInteractable::OnActivate_Implementation(ABattleEntity* entity)
{
	if (bIsTriggered)
		return;
	if (Cooldown > 0)
		return;
	bIsTriggered = true;
	Cooldown = CooldownTime;
	ParentGridCell->BattleManager->OnNewTurnEvent.AddDynamic(this, &ABaseInteractable::ReduceCooldown);
	OnActivate(entity);
	if (!bIsPersistent)
		Destroy();
}

void ABaseInteractable::RemoveFromGame()
{
	ParentGridCell->InteractableRef = nullptr;
	Destroy();
}
