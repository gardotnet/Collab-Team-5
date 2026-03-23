// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IHealthInteractable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UHealthInteractable : public UInterface
{
	GENERATED_BODY()
};

class TERMINAL_RECALL_API IHealthInteractable
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Health")
	void TakeCustomDamage(float damageTaken);
};