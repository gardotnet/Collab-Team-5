// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OurBasePlayerController.h"
#include "TraversalPlayerController.generated.h"

class UNodeInfoForCellSearch;
class AHarryWorldTraversalNode;
class ULevelSelectWindow;
class UInputMappingContext;
class AWorldTraversingEntityV2;

UCLASS()
class TERMINAL_RECALL_API ATraversalPlayerController : public AOurBasePlayerController
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, Category="Stuff For Designers | General Stuff")
	TSubclassOf<AWorldTraversingEntityV2> PlayerChar;
	UPROPERTY(EditAnywhere, Category="Stuff For Designers | General Stuff")
	float PlayerSpeed = 100;
	UPROPERTY(EditAnywhere, Category="Input Stuff")
	TObjectPtr<UInputMappingContext> MappingContext;

	bool ScreenOn = false;
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Necessities")
	TSubclassOf<ULevelSelectWindow> LevelSelectUIClass;
	UPROPERTY()
	TObjectPtr<ULevelSelectWindow> LevelSelectWindow;
	
	UPROPERTY()
	TObjectPtr<AWorldTraversingEntityV2> PlayerEntity;
	
	UPROPERTY()
	TObjectPtr<AHarryWorldTraversalNode> CurrentNode;
	UPROPERTY()
	TObjectPtr<AHarryWorldTraversalNode> DestinationNode;
	int32 PathIndex = 0;
	TArray<TObjectPtr<AHarryWorldTraversalNode>> ThePath;
	UPROPERTY()
	TArray<TObjectPtr<UNodeInfoForCellSearch>> OpenList;
	UPROPERTY()
	TArray<TObjectPtr<UNodeInfoForCellSearch>> CloseList;

	float MoveTimer = 0.0f;
	float DesiredMoveDuration = 0.0f;
public:
	UFUNCTION(BlueprintCallable, Category="Stuff For Designers | Functions")
	void OpenLevelSelect(int32 difficulty, const TArray<int32>& enemiesID);
	
	void NavigateWorld(TObjectPtr<AHarryWorldTraversalNode> cell);
	void UpdatePlayerPos(TObjectPtr<AHarryWorldTraversalNode> cell);
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	void NavigateOneSpaceBack();
protected:
	bool FindCell(TObjectPtr<UNodeInfoForCellSearch> currentInfo);
	// Utilities
	float Heuristic_Manhattan(FVector start, FVector end);
	UFUNCTION()
	void DelayedBeginPlay();
};
