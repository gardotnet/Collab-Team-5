// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NodeInfoForCellSearch.generated.h"

class AHarryWorldTraversalNode;
class AWorldGridCell;
/**
 * 
 */
UCLASS()
class TERMINAL_RECALL_API UNodeInfoForCellSearch : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	TObjectPtr<AWorldGridCell> Cell;
	UPROPERTY()
	TObjectPtr<AHarryWorldTraversalNode> Node;
	UPROPERTY()
	TObjectPtr<UNodeInfoForCellSearch> ParentCell;
	float GCost;
	float HCost;
	float FCost;
protected:
	UNodeInfoForCellSearch();
	UNodeInfoForCellSearch(TObjectPtr<AWorldGridCell> cell, TObjectPtr<UNodeInfoForCellSearch>, float gCost, float hCost);
public:
	void UpdateNodeInfo(TObjectPtr<UNodeInfoForCellSearch> parent, float gCost, float hCost);
};
