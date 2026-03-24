// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Terminal_Recall/StructsAndEnums/WorldNeighbourStruct.h"
#include "TraversalNode.generated.h"

class USplineComponent;

UCLASS()
class TERMINAL_RECALL_API ATraversalNode : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATraversalNode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal Node Info")
	TArray<FWorldNeighbourStruct> NeighbouringNodes;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Traversal Node Info")
	bool Visited = false;
	
	UPROPERTY(EditInstanceOnly, Category="Traversal Node Info", meta = (ToolTip = "Enable if node will trigger an encounter"))
	bool IsEncounter = true;
	
	UPROPERTY(EditInstanceOnly, Category="Traversal Node Info", meta = (ToolTip = "Paste the level's text file path here"))
	FString LevelFilePath;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<FWorldNeighbourStruct> OldNeighbouringNodes;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
};
