// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "WorldTraversingEntityV2.generated.h"

class UDefaultInstance;
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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Stuff For Designers | Variables")
	TArray<TObjectPtr<AHarryWorldTraversalNode>> TheNodePath;
	int32 PathIndex;
	
	UPROPERTY()
	TObjectPtr<USplineComponent> CurrentSplineComp;
	bool bDontReverseSplineLoc = true;
	
	FRotator TargetRotation;
	
	TArray<TObjectPtr<AHarryWorldTraversalNode>> TheReversePath;
	UPROPERTY()
	UDefaultInstance* TheGameInstance;

public:
	virtual void StartMove(TArray<TObjectPtr<AHarryWorldTraversalNode>> path) override;
	
	void MoveBack();

protected:
	
	virtual void BeginPlay() override;
	virtual void OnMove() override;
	UFUNCTION()
	virtual void OnSplineTraversal();
};
