// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BaseStatusEffect.generated.h"

class ABattleEntity;

UCLASS()
class TERMINAL_RECALL_API UBaseStatusEffect : public UObject
{
	GENERATED_BODY()
	
	//UFUNCTION(BlueprintNativeEvent, Category = "Stuff For Designers | Functions")
	void DoStatusEffect(ABattleEntity entityTarget);
	
};
