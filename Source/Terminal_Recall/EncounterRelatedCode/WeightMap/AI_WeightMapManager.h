// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>

#include "CoreMinimal.h"
#include "VisualWeightMapCell.h"
#include "GameFramework/Actor.h"
#include "Terminal_Recall/EncounterRelatedCode/BattleManager.h"
#include "Terminal_Recall/StructsAndEnums/GridRangeExport.h" // ADDED THIS LINE
#include "AI_WeightMapManager.generated.h"

// New USTRUCT to wrap TArray<FString> for UPROPERTY TMap values
USTRUCT(BlueprintType)
struct FActionDescriptionArray
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Info")
	TArray<FString> Descriptions;
};


UCLASS()
class TERMINAL_RECALL_API AAI_WeightMapManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAI_WeightMapManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Required Variables")
	ABattleManager* BattleManager;

	// Changed from TSet<FIntPoint> to TMap<FIntPoint, FActionDescriptionArray>
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Saved Information")
	TMap<FIntPoint, FActionDescriptionArray> possibleAttackTargetActions;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Saved Information")
	TMap<FIntPoint, int> WeightMap;

	UFUNCTION(BlueprintCallable, Category = "Generation")
	void GatherMoveableCells();

	UFUNCTION(BlueprintCallable, Category = "Generation")
	void GetCurrentActionPositionsForPlayer();

	UFUNCTION(BlueprintCallable, Category = "Generation")
	void GenerateWeightMap();

	UFUNCTION(BlueprintCallable, Category = "Generation")
	void ShowWeightMap();

	// New function for attack-from positions debug
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void ShowAttackFromPositionsDebug();

	// Helper function to get FIntPoint offset from ECompassDirection
	FIntPoint GetDirectionOffset(ECompassDirection Direction); // Added declaration here

	TMap<FIntPoint,TObjectPtr<ABattleGridCell>*> cellMap;

	FIntPoint LastPlayerPosition; // Added to track player movement

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignerAdjustable")
	int weightSize = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignerAdjustable")
	bool bshowWeightMap = true;

	// New debug boolean for attack range visualization
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignerAdjustable")
	bool bShowAttackRangeDebug = false;

	// New debug boolean for attack-from positions visualization
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignerAdjustable")
	bool bShowAttackFromPositionsDebug = false;

	// New debug boolean to control logging of final WeightMap content
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignerAdjustable")
	bool bLogFinalWeightMap = false;

	// New debug boolean to control verbose generation logging
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignerAdjustable")
	bool bLogVerboseGeneration = false;
};
