// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractablBaseClass.h"


// Sets default values
AInteractablBaseClass::AInteractablBaseClass()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemName = TEXT("Default Item");
	ItemDescription = TEXT("Default Description");
}

// Called when the game starts or when spawned
void AInteractablBaseClass::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractablBaseClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractablBaseClass::PickUp_Implementation()
{
	// Default Implementation for picking up the object
	// Can be extended or fully overridden in Blueprints
	Destroy();
}

void AInteractablBaseClass::GetInteractableInfo(FString& OutName, FString& OutDescription) const
{
	OutName = ItemName;
	OutDescription = ItemDescription;
}

void AInteractablBaseClass::OnInteract_Implementation()
{
	// Base implementation for interaction, to be overridden by subclasses or Blueprints
}
