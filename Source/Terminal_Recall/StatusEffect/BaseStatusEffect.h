// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseStatusEffect.generated.h"

class ABattleEntity;

UCLASS()
class TERMINAL_RECALL_API ABaseStatusEffect : public AActor
{
	GENERATED_BODY()
	
public:
	ABaseStatusEffect();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stuff For Designers | Variables")
	FString StatusEffectName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stuff For Designers | Variables")
	int Duration;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Stuff For Designers | Functions")
	void DoStatusEffect(ABattleEntity* entityTarget);

	UFUNCTION(BlueprintImplementableEvent, Category = "Stuff For Designers | Functions")
	void OnEffectEnded();
};
