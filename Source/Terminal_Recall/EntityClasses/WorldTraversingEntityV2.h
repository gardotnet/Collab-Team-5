// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "WorldTraversingEntityV2.generated.h"

class ATraversalPlayerController;
class AHarryWorldTraversalNode;

UCLASS()
class TERMINAL_RECALL_API AWorldTraversingEntityV2 : public ABaseEntity
{
	GENERATED_BODY()

public:
	AWorldTraversingEntityV2();
	
protected:
	
	UPROPERTY()
	TObjectPtr<ATraversalPlayerController> PlayerController;
	
	TArray<TObjectPtr<AHarryWorldTraversalNode>> TheNodePath;
	int32 PathIndex;

public:
	virtual void StartMove(TArray<TObjectPtr<AHarryWorldTraversalNode>> path) override;

protected:
	
	virtual void BeginPlay() override;

	
};
