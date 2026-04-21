// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualWeightMapCell.h"


// Sets default values
AVisualWeightMapCell::AVisualWeightMapCell()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AVisualWeightMapCell::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVisualWeightMapCell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

