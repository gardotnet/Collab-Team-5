// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NodeInfoForCellSearch.generated.h"

class AWorldGridCell;
/**
 * 
 */
UCLASS()
class TERMINAL_RECALL_API UNodeInfoForCellSearch : public UObject
{
	GENERATED_BODY()
	
public:
	TObjectPtr<AWorldGridCell> Cell;
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
