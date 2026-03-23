// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameStateHandle.generated.h"

class ATraversalNode;

/**
 * 
 */
UCLASS()
class TERMINAL_RECALL_API AGameStateHandle : public AGameStateBase
{
	GENERATED_BODY()

public:
	// A registry or manager for nodes and their neighbors could be here
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Traversal System")
	TArray<ATraversalNode*> AllNodes;

	// Potentially helper functions to manage neighbor updates
};