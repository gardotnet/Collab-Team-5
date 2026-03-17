// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseTrapLatest.h"


// Sets default values
ABaseTrapLatest::ABaseTrapLatest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseTrapLatest::BeginPlay()
{
	Super::BeginPlay();
	
	FetchTrapData();
}

// Called every frame
void ABaseTrapLatest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseTrapLatest::FetchTrapData()
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

void ABaseTrapLatest::OnUnitArrive_Implementation()
{
	// Trap activation logic here
}
