// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DefaultInstance.generated.h"

class ABaseInteractable;

UCLASS()
class TERMINAL_RECALL_API UDefaultInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stuff For Designers | System Variables")
	FVector PlayerSpawnPos = FVector::Zero();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stuff For Designers | System Variables")
	FString Map = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stuff For Designers | System Variables")
	TArray<FVector> VisitedCells;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stuff For Designers | Player Variables")
	float Health = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stuff For Designers | Player Variables")
	float MaxHealth = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stuff For Designers | Player Variables")
	float DamageMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stuff For Designers | Player Variables")
	TArray<int> AttackList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stuff For Designers | Player Variables")
	TArray<int> MoveList;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stuff For Designers | Player Variables")
	FVector LastValidVisitCell;
	
	TSubclassOf<ABaseInteractable> RobotPickUpClass;
	FIntPoint PickUpLoc = FIntPoint(0, 0);

protected:
	#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif

	virtual void OnStart() override;
public:
	void EncounterWon();
};
