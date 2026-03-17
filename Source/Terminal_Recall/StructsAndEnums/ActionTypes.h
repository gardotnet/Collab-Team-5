// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionTypes.generated.h"

UENUM()
enum class EActionType : uint8
{
	None UMETA(DisplayName = "None"),
	Movement UMETA(DisplayName = "Movement"),
	Attack UMETA(DisplayName = "Attack"),
	Item UMETA(DisplayName = "Item")
};
