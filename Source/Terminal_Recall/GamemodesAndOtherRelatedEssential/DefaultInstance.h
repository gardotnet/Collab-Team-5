// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Terminal_Recall/StructsAndEnums/ActionTypes.h"
#include "Terminal_Recall/StructsAndEnums/ForDataTable/ActionStruct.h"
#include "DefaultInstance.generated.h"

/**
 * 
 */
UCLASS()
class TERMINAL_RECALL_API UDefaultInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="For Designers | System Variables")
	FVector PlayerSpawnPos = FVector::Zero();
	UPROPERTY(EditAnywhere, Category="For Designers | System Variables")
	FString Map = "";
	
	UPROPERTY(EditAnywhere, Category="For Designers | Player Variables")
	float Health = 50.0f;
	UPROPERTY(EditAnywhere, Category="For Designers | Player Variables")
	float DamageMultiplier = 1.0f;
	
	UPROPERTY(EditAnywhere, Category="For Designers | Player Variables")
	TArray<int> AttackList;
	
	UPROPERTY(EditAnywhere, Category="For Designers | Player Variables")
	TArray<int> MoveList;
};
