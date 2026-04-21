// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VisualWeightMapCell.generated.h"

UCLASS()
class TERMINAL_RECALL_API AVisualWeightMapCell : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVisualWeightMapCell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignerAdjustable")
	int maxGradient = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignerAdjustable")
	int chosenGradient = 0;
};
