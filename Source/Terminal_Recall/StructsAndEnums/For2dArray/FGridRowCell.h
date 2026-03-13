// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FGridRowCell.generated.h"

class ABattleGridCell;

USTRUCT(BlueprintType)
struct FGridRowCell
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<ABattleGridCell>> Elements;

	void Add(TObjectPtr<ABattleGridCell> element) { Elements.Add(element); }
	int32 Num() { return Elements.Num(); }
	TObjectPtr<ABattleGridCell>& operator[](int32 element) { return Elements[element]; }
};
