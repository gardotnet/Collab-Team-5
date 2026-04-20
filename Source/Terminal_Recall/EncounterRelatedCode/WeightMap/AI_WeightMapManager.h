// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Terminal_Recall/EncounterRelatedCode/BattleManager.h"
#include "AI_WeightMapManager.generated.h"

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Saved Information")
	TArray<FIntPoint> possibleTravelPosition;

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

	TMap<FIntPoint,TObjectPtr<ABattleGridCell>*> cellMap;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignerAdjustable")
	int weightSize = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignerAdjustable")
	bool bshowWeightMap = true;
};
