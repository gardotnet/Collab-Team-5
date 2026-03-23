// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WorldNeighbourStruct.generated.h"

class ATraversalNode;
class USplineComponent;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FWorldNeighbourStruct : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<ATraversalNode> Node;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(tooltip = "Scaled by 1 Block (100 Units)"))
	int32 Length = 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool HasPathConnected = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> PathObject;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USplineComponent> PathSplineComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<AActor>> SpawnedPathActors;
};
