// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Terminal_Recall/StructsAndEnums/HarryNeighbourStruct.h"
#include "HarryWorldTraversalSystem.generated.h"

class APathActor;
class AHarryWorldTraversalNode;

UCLASS()
class TERMINAL_RECALL_API AHarryWorldTraversalSystem : public AActor
{
	GENERATED_BODY()

public:
	#pragma region Path Stuff
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal System | Generation", meta = (ToolTip = "Add the path actor here"))
	TSubclassOf<APathActor> SplinePathBpClass;
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

	#pragma region Actor Spawning on Path
	
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal System | Actor Spawning", meta = (ToolTip = "Maximum number of actors that can spawn on a single path connection", EditCondition="bSpawnActorsOnPath", UIMin = "1"))
	int32 MaxActorsToSpawn = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal System | Actor Spawning", meta = (ToolTip = "Minimum distance along the spline (0.0 to 1.0) where actors can spawn. E.g. 0.2 means no actors in the first 20% of the path.", EditCondition="bSpawnActorsOnPath", UIMin = "0.0", UIMax = "1.0"))
	float MinSpawnDistancePercentage = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal System | Actor Spawning", meta = (ToolTip = "Maximum distance along the spline (0.0 to 1.0) where actors can spawn. E.g. 0.8 means no actors in the last 20% of the path.", EditCondition="bSpawnActorsOnPath", UIMin = "0.0", UIMax = "1.0"))
	float MaxSpawnDistancePercentage = 0.8f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal System | Actor Spawning", meta = (ToolTip = "If spawning multiple actors, randomizes the yaw (Z rotation) for each actor.", EditCondition="bSpawnActorsOnPath"))
	bool bRandomizeActorYaw = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal System | Actor Spawning", meta = (ToolTip = "Align the actor's forward axis to the spline's curve direction?", EditCondition="bSpawnActorsOnPath"))
	bool bAlignActorsToSplineRotation = true;
	
	#pragma endregion Actor Spawning on Path

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal System | Data Saving", meta = (ToolTip = "The name of the file to save/load spline data from (e.g. 'Level1_Splines')", GetOptions="GetAvailableSaveFiles"))
	FString SaveFileName = "DefaultSplineData";*/
	
	#pragma endregion Path Stuff

public:
	AHarryWorldTraversalSystem();

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

	// Update spline meshes along the splines
	void RebuildMeshesForPath(FHarryNeighbourStruct* NeighbourInfo);
	void ActualPathCreation(AHarryWorldTraversalNode* NodeA, FHarryNeighbourStruct* NeighbourInfo);
	
	/*// Save spline data to JSON
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Traversal System | Data Saving")
	void SaveSplineData();

	// Load spline data from JSON
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Traversal System | Data Saving")
	void LoadSplineData();*/

protected:
	void ClearSpawnedActors(FHarryNeighbourStruct* NeighbourInfo);
};
