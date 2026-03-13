// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EncounterLevelMakerManager.generated.h"

class AGridVisualiser;

UCLASS()
class TERMINAL_RECALL_API AEncounterLevelMakerManager : public AActor
{
	GENERATED_BODY()

public:
	AEncounterLevelMakerManager();

	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	TSubclassOf<AGridVisualiser> GridDisplayerClass;

	UFUNCTION(BlueprintCallable, CallInEditor, Category= "For Designers | Tools")
	void SpawnGrid();
	
	UFUNCTION(BlueprintCallable, Category= "For Designers | Functions")
	void CreateGrid(FIntPoint gridSize);
	
protected:
	UPROPERTY()
	TObjectPtr<AGridVisualiser> GridDisplayerObject;
};
