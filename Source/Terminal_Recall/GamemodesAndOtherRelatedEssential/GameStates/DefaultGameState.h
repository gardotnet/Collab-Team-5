// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DefaultGameState.generated.h"

/**
 * 
 */
UCLASS()
class TERMINAL_RECALL_API ADefaultGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="Variables (For Designers)")
	FVector PlayerSpawnPos = FVector::Zero();
};
