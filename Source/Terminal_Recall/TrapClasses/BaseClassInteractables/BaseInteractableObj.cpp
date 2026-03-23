// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInteractableObj.h"


ABaseInteractableObj::ABaseInteractableObj()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseInteractableObj::BeginPlay()
{
	Super::BeginPlay();
	
	ItemName = TEXT("Default Item");
	ItemDescription = TEXT("Default Description");

	FetchTrapData();
}

// Called every frame
void ABaseInteractableObj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseInteractableObj::FetchTrapData()
{
	if (TrapDataTable && !TrapRowName.IsNone())
	{
		FTrapInfoStruct* Row = TrapDataTable->FindRow<FTrapInfoStruct>(TrapRowName, TEXT("FetchTrapData"));
		if (Row)
		{
			TrapData = *Row;
		}
	}
}

void ABaseInteractableObj::OnUnitArrive_Implementation()
{

}

void ABaseInteractableObj::PickUp_Implementation()
{
	Destroy();
}

void ABaseInteractableObj::GetInteractableInfo(FString& OutName, FString& OutDescription) const
{
	OutName = ItemName;
	OutDescription = ItemDescription;
}

void ABaseInteractableObj::OnInteract_Implementation()
{
	
}

void ABaseInteractableObj::DamageEntity(AActor* interactedActor, float amount)
{
	if (interactedActor && interactedActor->Implements<UHealthInteractable>())
	{
		IHealthInteractable::Execute_TakeCustomDamage(this, amount, interactedActor);
	}
}
