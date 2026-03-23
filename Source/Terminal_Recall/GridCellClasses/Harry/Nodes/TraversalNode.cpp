// Fill out your copyright notice in the Description page of Project Settings.


#include "TraversalNode.h"


// Sets default values
ATraversalNode::ATraversalNode()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATraversalNode::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATraversalNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

