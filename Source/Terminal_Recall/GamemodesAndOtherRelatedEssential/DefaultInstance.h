// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Terminal_Recall/StructsAndEnums/ActionTypes.h"
#include "DefaultInstance.generated.h"

/**
 * 
 */
UCLASS()
class TERMINAL_RECALL_API UDefaultInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="For Designers | Variables")
	FVector PlayerSpawnPos = FVector::Zero();
	UPROPERTY(EditAnywhere, Category="For Designers | Variables")
	FString Map = "";
	UPROPERTY(EditAnywhere, Category="For Designers | Variables")
	TArray<EAttackActionTypes> PlayerMoves;
	
};
