// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "WorldTraversingEntityV2.generated.h"

class USplineComponent;
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
	
	UPROPERTY()
	TObjectPtr<USplineComponent> CurrentSplineComp;
	bool bDontReverseSplineLoc = true;
	
	FRotator TargetRotation;
	

public:
	virtual void StartMove(TArray<TObjectPtr<AHarryWorldTraversalNode>> path) override;


protected:
	
	virtual void BeginPlay() override;
	virtual void OnMove() override;
	UFUNCTION()
	virtual void OnSplineTraversal();

	
};
