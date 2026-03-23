// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Nodes/TraversalNode.h"
#include "Components/SplineMeshComponent.h"
#include "WorldTraversalNodeSystem.generated.h"

class USplineComponent;
class UStaticMesh;

UCLASS()
class TERMINAL_RECALL_API AWorldTraversalNodeSystem : public AActor
{
	GENERATED_BODY()

public:
	AWorldTraversalNodeSystem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal System | Generation", meta = (ToolTip = "Add the path actor here"))
	TSubclassOf<AActor> PathBpClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal System | Mesh Info", meta = (ToolTip = "The mesh to populate along the spline"))
	TObjectPtr<UStaticMesh> PathMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal System | Mesh Info", meta = (ToolTip = "Forward axis for the spline mesh (usually X or Y)"))
	TEnumAsByte<ESplineMeshAxis::Type> PathMeshForwardAxis;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal System | Mesh Info", meta = (ToolTip = "Cross-section scale for the spline mesh instances. X and Y affect width/height based on forward axis."))
	FVector2D PathMeshScale = FVector2D(1.0, 1.0);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal System | Mesh Info", meta = (ToolTip = "Offset for the spline mesh instances"))
	FVector2D PathMeshOffset = FVector2D(0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal System | Mesh Info", meta = (ToolTip = "Number of mesh segments to spawn between each spline point pair. Higher number = smoother curve, but more expensive.", UIMin = "1", UIMax = "20"))
	int32 MeshResolution = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal System | Actor Spawning", meta = (ToolTip = "Should we spawn an actor along the path?"))
	bool bSpawnActorsOnPath = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal System | Actor Spawning", meta = (ToolTip = "The actor class to spawn along the path", EditCondition="bSpawnActorsOnPath"))
	TSubclassOf<AActor> ActorToSpawnClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal System | Actor Spawning", meta = (ToolTip = "Offset for spawned actors", EditCondition="bSpawnActorsOnPath"))
	FVector SpawnedActorOffset = FVector(0, 0, 0);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal System | Actor Spawning", meta = (ToolTip = "Scale for spawned actors", EditCondition="bSpawnActorsOnPath"))
	FVector SpawnedActorScale = FVector(1, 1, 1);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal System | Actor Spawning", meta = (ToolTip = "Chance out of 100 to spawn exactly one actor on the path (if enabled)", EditCondition="bSpawnActorsOnPath"))
	float SpawnChance = 100.f;

	// Reset path for all traversal nodes
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Traversal System | Tools")
	void ResetPathPosition();
	
	// Create paths for all traversal nodes
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Traversal System | Tools")
	void CreatePaths();
	
	// Update spline meshes along the splines
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Traversal System | Tools")
	void UpdatePathMeshes();
	
	// Clear all paths
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Traversal System | Tools")
	void ClearPaths();

protected:
	virtual void BeginPlay() override;

	void ActualPathCreation(ATraversalNode* NodeA, FWorldNeighbourStruct* NeighbourInfo);
	void RebuildMeshesForPath(FWorldNeighbourStruct* NeighbourInfo);
	void ClearSpawnedActors(FWorldNeighbourStruct* NeighbourInfo);

public:
	virtual void Tick(float DeltaTime) override;
};
