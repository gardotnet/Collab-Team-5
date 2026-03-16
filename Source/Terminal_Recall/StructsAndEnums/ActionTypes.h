// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionTypes.generated.h"

UENUM(BlueprintType)
enum class EAttackActionTypes : uint8
{
	None UMETA(DisplayName = "None"),
	King UMETA(DisplayName = "King", ToolTip = "one space 8 way movement"),
	Queen UMETA(DisplayName = "Queen", ToolTip = "8 way movement"),
	Bishop UMETA(DisplayName = "Bishop", ToolTip = "Diagonal movement"),
	Rook UMETA(DisplayName = "Rook", ToolTip = "Orthogonal movement"),
	Knight UMETA(DisplayName = "Knight", ToolTip = "Take an 'L'"),
	Custom UMETA(DisplayName = "Custom", ToolTip = "A custom attack stored in 'Attack Data Table'")
};

enum class EActionType : uint8
{
	None UMETA(DisplayName = "None"),
	Movement UMETA(DisplayName = "Movement"),
	Attack UMETA(DisplayName = "Attack"),
	Item UMETA(DisplayName = "Item")
};
