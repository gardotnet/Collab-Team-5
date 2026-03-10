// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BattleCellState.generated.h"

UENUM(BlueprintType)
enum class EBattleCellState : uint8
{
	None UMETA(DisplayName = "None"),
	Move UMETA(DisplayName = "Move"),
	Attack UMETA(DisplayName = "Attack")
};
