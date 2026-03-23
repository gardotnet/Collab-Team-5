// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldTraversalNodeSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Nodes/TraversalNode.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Terminal_Recall/StructsAndEnums/WorldNeighbourStruct.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonReader.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"

// Sets default values
AWorldTraversalNodeSystem::AWorldTraversalNodeSystem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PathMeshForwardAxis = ESplineMeshAxis::X;
}

// Called when the game starts or when spawned
void AWorldTraversalNodeSystem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWorldTraversalNodeSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWorldTraversalNodeSystem::CreatePaths()
{
#if WITH_EDITOR
	TArray<AActor*> FoundNodes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATraversalNode::StaticClass(), FoundNodes);
	
	for (AActor* NodeActor : FoundNodes)
	{
		ATraversalNode* Node = Cast<ATraversalNode>(NodeActor);
		if (!Node) continue;
		
		for (FWorldNeighbourStruct& Neighbour : Node->NeighbouringNodes)
		{
			if (Neighbour.Node != nullptr)
			{
				ActualPathCreation(Node, &Neighbour);
			}
		}
	}
	UpdatePathMeshes();
#endif
}

void AWorldTraversalNodeSystem::ActualPathCreation(ATraversalNode* NodeA, FWorldNeighbourStruct* NeighbourInfo)
{
#if WITH_EDITOR
	if (!NodeA || !NeighbourInfo || !NeighbourInfo->Node) return;
	
	ATraversalNode* NodeB = NeighbourInfo->Node;

	if (NeighbourInfo->HasPathConnected)
		return;

	NeighbourInfo->HasPathConnected = true;

	// Check if NodeB already has NodeA as a neighbour and connected
	FWorldNeighbourStruct* FoundSelf = NodeB->NeighbouringNodes.FindByPredicate([NodeA](const FWorldNeighbourStruct& Item)
	{
		return Item.Node == NodeA;
	});

	if (FoundSelf && FoundSelf->HasPathConnected)
	{
		UE_LOG(LogTemp, Warning, TEXT("Path already connected between %s and %s"), *NodeA->GetName(), *NodeB->GetName());
		return;
	}

	if (!FoundSelf)
	{
		// We need to add NodeA to NodeB's neighbours
		FWorldNeighbourStruct NewStruct;
		NewStruct.Node = NodeA;
		NodeB->NeighbouringNodes.Add(NewStruct);
		
		FoundSelf = NodeB->NeighbouringNodes.FindByPredicate([NodeA](const FWorldNeighbourStruct& Item)
		{
			return Item.Node == NodeA;
		});
	}

	if (FoundSelf)
	{
		FoundSelf->HasPathConnected = true;
	}

	// Calculate the position of the path (spline)
	FVector PosA = NodeA->GetActorLocation();
	FVector PosB = NodeB->GetActorLocation();
	PosB.Z = PosA.Z; // keep on a 2D plane (same height)
	
	float Dist = FVector::Dist(PosA, PosB);
	
	FVector SpawnPoint = (PosA + PosB) * 0.5f;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AActor* NewPath = GetWorld()->SpawnActor<AActor>(PathBpClass, SpawnPoint, FRotator::ZeroRotator, SpawnParams);
	
	if (NewPath)
	{
		NewPath->Tags.Add(FName("TraversalPathActor"));
		
		USplineComponent* SplineComp = Cast<USplineComponent>(NewPath->GetComponentByClass(USplineComponent::StaticClass()));
		if (!SplineComp)
		{
			SplineComp = NewObject<USplineComponent>(NewPath, USplineComponent::StaticClass(), TEXT("PathSpline"));
			if (SplineComp)
			{
				SplineComp->RegisterComponent();
				NewPath->AddInstanceComponent(SplineComp);
			}
		}

		if (SplineComp)
		{
			SplineComp->ClearSplinePoints(true);
			SplineComp->AddSplinePoint(PosA, ESplineCoordinateSpace::World, true);
			SplineComp->AddSplinePoint(PosB, ESplineCoordinateSpace::World, true);
		}

		NeighbourInfo->PathObject = NewPath;
		NeighbourInfo->PathSplineComponent = SplineComp;
		NeighbourInfo->Length = Dist / 100;

		if (FoundSelf)
		{
			FoundSelf->PathObject = NewPath;
			FoundSelf->PathSplineComponent = SplineComp;
			FoundSelf->Length = Dist / 100;
		}
	}
#endif
}

void AWorldTraversalNodeSystem::ResetPathPosition()
{
#if WITH_EDITOR
	TArray<AActor*> FoundNodes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATraversalNode::StaticClass(), FoundNodes);

	for (AActor* NodeActor : FoundNodes)
	{
		ATraversalNode* Node = Cast<ATraversalNode>(NodeActor);
		if (!Node) continue;

		for (FWorldNeighbourStruct& Neighbour : Node->NeighbouringNodes)
		{
			if (Neighbour.Node == nullptr) continue;

			FVector PosA = Node->GetActorLocation();
			FVector PosB = Neighbour.Node->GetActorLocation();
			PosB.Z = PosA.Z;

			float Dist = FVector::Dist(PosA, PosB);
			FVector MidPoint = (PosA + PosB) * 0.5f;

			if (IsValid(Neighbour.PathObject))
			{
				// We don't change rotation because splines handle the curve
				Neighbour.PathObject->SetActorLocation(MidPoint);
				
				if (Neighbour.PathSplineComponent)
				{
					// Maintain the first and last points at the node locations
					int32 NumPoints = Neighbour.PathSplineComponent->GetNumberOfSplinePoints();
					if (NumPoints >= 2)
					{
						Neighbour.PathSplineComponent->SetLocationAtSplinePoint(0, PosA, ESplineCoordinateSpace::World, true);
						Neighbour.PathSplineComponent->SetLocationAtSplinePoint(NumPoints - 1, PosB, ESplineCoordinateSpace::World, true);
					}
				}
			}
			else
			{
				// Re-create path if missing
				ActualPathCreation(Node, &Neighbour);
			}
		}
	}
	UpdatePathMeshes();
#endif
}

void AWorldTraversalNodeSystem::UpdatePathMeshes()
{
#if WITH_EDITOR
	TArray<AActor*> FoundNodes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATraversalNode::StaticClass(), FoundNodes);

	for (AActor* NodeActor : FoundNodes)
	{
		ATraversalNode* Node = Cast<ATraversalNode>(NodeActor);
		if (!Node) continue;

		for (FWorldNeighbourStruct& Neighbour : Node->NeighbouringNodes)
		{
			if (IsValid(Neighbour.PathObject) && Neighbour.PathSplineComponent)
			{
				RebuildMeshesForPath(&Neighbour);
			}
		}
	}
#endif
}

void AWorldTraversalNodeSystem::ClearPaths()
{
#if WITH_EDITOR
	TArray<AActor*> FoundNodes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATraversalNode::StaticClass(), FoundNodes);

	// First pass: clear all references from the nodes
	for (AActor* NodeActor : FoundNodes)
	{
		ATraversalNode* Node = Cast<ATraversalNode>(NodeActor);
		if (!Node) continue;

		for (int32 i = 0; i < Node->NeighbouringNodes.Num(); i++)
		{
			FWorldNeighbourStruct& Neighbour = Node->NeighbouringNodes[i];
			
			ClearSpawnedActors(&Neighbour);
			
			if (IsValid(Neighbour.PathObject))
			{
				Neighbour.PathObject->Destroy();
			}
			
			Neighbour.PathObject = nullptr;
			Neighbour.PathSplineComponent = nullptr;
			Neighbour.HasPathConnected = false;
		}
		
		Node->Modify();
	}
	
	// Second pass: Destroy all path actors to catch any orphaned ones
	TArray<AActor*> FoundPaths;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("TraversalPathActor"), FoundPaths);
	for (AActor* Path : FoundPaths)
	{
		if (IsValid(Path))
		{
			Path->Destroy();
		}
	}
	
	// Third pass: Destroy all spawned actor elements along the path
	TArray<AActor*> FoundSpawnedActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("TraversalSpawnedActor"), FoundSpawnedActors);
	for (AActor* SpawnedActor : FoundSpawnedActors)
	{
		if (IsValid(SpawnedActor))
		{
			SpawnedActor->Destroy();
		}
	}
#endif
}

void AWorldTraversalNodeSystem::ClearSpawnedActors(FWorldNeighbourStruct* NeighbourInfo)
{
#if WITH_EDITOR
	if (!NeighbourInfo) return;
	for (AActor* Spawned : NeighbourInfo->SpawnedPathActors)
	{
		if (IsValid(Spawned))
		{
			Spawned->Destroy();
		}
	}
	NeighbourInfo->SpawnedPathActors.Empty();
#endif
}

void AWorldTraversalNodeSystem::RebuildMeshesForPath(FWorldNeighbourStruct* NeighbourInfo)
{
#if WITH_EDITOR
	if (!NeighbourInfo || !IsValid(NeighbourInfo->PathObject) || !NeighbourInfo->PathSplineComponent) return;

	AActor* PathActor = NeighbourInfo->PathObject;
	USplineComponent* Spline = NeighbourInfo->PathSplineComponent;

	// Remove old meshes
	TArray<USplineMeshComponent*> ExistingMeshes;
	PathActor->GetComponents<USplineMeshComponent>(ExistingMeshes);
	for (USplineMeshComponent* MeshComp : ExistingMeshes)
	{
		if (IsValid(MeshComp))
		{
			MeshComp->DestroyComponent();
		}
	}
	
	// Remove old actors
	ClearSpawnedActors(NeighbourInfo);

	int32 NumPoints = Spline->GetNumberOfSplinePoints();
	if (NumPoints < 2) return;

	// Spawn actors if enabled
	if (bSpawnActorsOnPath && ActorToSpawnClass)
	{
		int32 ActualSpawns = 0;
		float SplineLength = Spline->GetSplineLength();
		
		// Ensure valid ranges
		float MinClamp = FMath::Clamp(MinSpawnDistancePercentage, 0.f, 1.f);
		float MaxClamp = FMath::Clamp(MaxSpawnDistancePercentage, MinClamp, 1.f);
		
		float MinDist = SplineLength * MinClamp;
		float MaxDist = SplineLength * MaxClamp;

		int32 Iterations = FMath::Max(1, MaxActorsToSpawn);
		
		for (int32 i = 0; i < Iterations; i++)
		{
			float RandomRoll = FMath::RandRange(0.f, 100.f);
			if (RandomRoll <= SpawnChance)
			{
				// Pick a random distance along the valid subsection of the spline
				float RandomDistance = FMath::RandRange(MinDist, MaxDist);

				// Calculate final transform based on the spline at this random distance
				FVector Location = Spline->GetLocationAtDistanceAlongSpline(RandomDistance, ESplineCoordinateSpace::World);
				FRotator Rotation = Spline->GetRotationAtDistanceAlongSpline(RandomDistance, ESplineCoordinateSpace::World);

				if (!bAlignActorsToSplineRotation)
				{
					Rotation = FRotator::ZeroRotator;
				}

				if (bRandomizeActorYaw)
				{
					Rotation.Yaw += FMath::RandRange(0.f, 360.f);
				}

				// Add custom offset (rotated by the calculated rotation at this point to stay consistent)
				Location += Rotation.RotateVector(SpawnedActorOffset);

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				
				AActor* Spawned = GetWorld()->SpawnActor<AActor>(ActorToSpawnClass, Location, Rotation, SpawnParams);
				if (Spawned)
				{
					Spawned->Tags.Add(FName("TraversalSpawnedActor"));
					Spawned->SetActorScale3D(SpawnedActorScale);
					NeighbourInfo->SpawnedPathActors.Add(Spawned);
					
					// Attach to the path object to keep the hierarchy clean
					Spawned->AttachToActor(PathActor, FAttachmentTransformRules::KeepWorldTransform);
					ActualSpawns++;
				}
			}
			
			if (ActualSpawns >= MaxActorsToSpawn) break;
		}
	}

	// Always generate meshes if mesh is set
	if (PathMesh)
	{
		int32 ValidRes = FMath::Max(1, MeshResolution);

		for (int32 i = 0; i < NumPoints - 1; i++)
		{
			// Instead of stretching one mesh per spline point pair, we break it down into valid resolution chunks
			float DistStart = Spline->GetDistanceAlongSplineAtSplinePoint(i);
			float DistEnd = Spline->GetDistanceAlongSplineAtSplinePoint(i + 1);
			float SectionLength = DistEnd - DistStart;
			float SegmentLength = SectionLength / ValidRes;

			for (int32 j = 0; j < ValidRes; j++)
			{
				float CurrentStartDist = DistStart + (j * SegmentLength);
				float CurrentEndDist = CurrentStartDist + SegmentLength;

				USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(PathActor, USplineMeshComponent::StaticClass());
				if (SplineMesh)
				{
					SplineMesh->SetStaticMesh(PathMesh);
					SplineMesh->SetForwardAxis(PathMeshForwardAxis);
					SplineMesh->SetMobility(EComponentMobility::Movable); 
					
					// Using SetStartScale and SetEndScale properly scales the cross-section without destroying the start/end locations
					SplineMesh->SetStartScale(PathMeshScale, false);
					SplineMesh->SetEndScale(PathMeshScale, false);
					
					// Spline offset without destroying placement
					SplineMesh->SetStartOffset(PathMeshOffset, false);
					SplineMesh->SetEndOffset(PathMeshOffset, false);
					
					SplineMesh->RegisterComponent();
					PathActor->AddInstanceComponent(SplineMesh);
					SplineMesh->AttachToComponent(Spline, FAttachmentTransformRules::KeepRelativeTransform);

					// Calculate Local Space locations and tangents at exact distances along the curve
					// We clamp just in case floating point inaccuracies push it past spline length
					float SplineMaxLen = Spline->GetSplineLength();
					float SafeStart = FMath::Clamp(CurrentStartDist, 0.f, SplineMaxLen);
					float SafeEnd = FMath::Clamp(CurrentEndDist, 0.f, SplineMaxLen);

					FVector StartPos = Spline->GetLocationAtDistanceAlongSpline(SafeStart, ESplineCoordinateSpace::Local);
					FVector StartTangent = Spline->GetTangentAtDistanceAlongSpline(SafeStart, ESplineCoordinateSpace::Local);
					
					FVector EndPos = Spline->GetLocationAtDistanceAlongSpline(SafeEnd, ESplineCoordinateSpace::Local);
					FVector EndTangent = Spline->GetTangentAtDistanceAlongSpline(SafeEnd, ESplineCoordinateSpace::Local);

					// Because we're chopping up the spline section, we must scale down the tangents proportionally
					// otherwise the curves inside the segment will bulge wildly.
					StartTangent = StartTangent.GetSafeNormal() * SegmentLength;
					EndTangent = EndTangent.GetSafeNormal() * SegmentLength;

					SplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, true);
				}
			}
		}
	}
#endif
}

TArray<FString> AWorldTraversalNodeSystem::GetAvailableSaveFiles() const
{
	TArray<FString> FileNames;
#if WITH_EDITOR
	FString SaveDir = FPaths::ProjectSavedDir();
	IFileManager& FileManager = IFileManager::Get();

	TArray<FString> Files;
	FileManager.FindFiles(Files, *(SaveDir / TEXT("*.json")), true, false);

	for (const FString& File : Files)
	{
		// Remove the .json extension to display just the name
		FileNames.Add(FPaths::GetBaseFilename(File));
	}

	if (FileNames.Num() == 0)
	{
		FileNames.Add(TEXT("DefaultSplineData"));
	}
#endif
	return FileNames;
}

void AWorldTraversalNodeSystem::SaveSplineData()
{
#if WITH_EDITOR
	TArray<AActor*> FoundNodes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATraversalNode::StaticClass(), FoundNodes);

	TArray<TSharedPtr<FJsonValue>> JsonPathsArray;

	for (AActor* NodeActor : FoundNodes)
	{
		ATraversalNode* Node = Cast<ATraversalNode>(NodeActor);
		if (!Node) continue;

		for (FWorldNeighbourStruct& Neighbour : Node->NeighbouringNodes)
		{
			if (Neighbour.Node != nullptr && IsValid(Neighbour.PathSplineComponent))
			{
				TSharedPtr<FJsonObject> PathObject = MakeShared<FJsonObject>();
				PathObject->SetStringField("NodeA", Node->GetName());
				PathObject->SetStringField("NodeB", Neighbour.Node->GetName());

				TArray<TSharedPtr<FJsonValue>> PointsArray;
				USplineComponent* Spline = Neighbour.PathSplineComponent;

				for (int32 i = 0; i < Spline->GetNumberOfSplinePoints(); i++)
				{
					TSharedPtr<FJsonObject> PointObject = MakeShared<FJsonObject>();
					
					FVector Loc = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
					TSharedPtr<FJsonObject> LocObj = MakeShared<FJsonObject>();
					LocObj->SetNumberField("X", Loc.X);
					LocObj->SetNumberField("Y", Loc.Y);
					LocObj->SetNumberField("Z", Loc.Z);
					PointObject->SetObjectField("Location", LocObj);
					
					FVector Tangent = Spline->GetArriveTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
					TSharedPtr<FJsonObject> TanObj = MakeShared<FJsonObject>();
					TanObj->SetNumberField("X", Tangent.X);
					TanObj->SetNumberField("Y", Tangent.Y);
					TanObj->SetNumberField("Z", Tangent.Z);
					PointObject->SetObjectField("Tangent", TanObj);

					PointsArray.Add(MakeShared<FJsonValueObject>(PointObject));
				}

				PathObject->SetArrayField("SplinePoints", PointsArray);
				JsonPathsArray.Add(MakeShared<FJsonValueObject>(PathObject));
			}
		}
	}

	TSharedPtr<FJsonObject> RootObject = MakeShared<FJsonObject>();
	RootObject->SetArrayField("Paths", JsonPathsArray);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

	FString FilePath = FPaths::ProjectSavedDir() / SaveFileName + TEXT(".json");
	if (FFileHelper::SaveStringToFile(OutputString, *FilePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("Spline Data Successfully Saved to %s"), *FilePath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save Spline Data to %s"), *FilePath);
	}
#endif
}

void AWorldTraversalNodeSystem::LoadSplineData()
{
#if WITH_EDITOR
	FString FilePath = FPaths::ProjectSavedDir() / SaveFileName + TEXT(".json");
	FString JsonString;

	if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Spline Data from %s. Make sure the file exists."), *FilePath);
		return;
	}

	TSharedPtr<FJsonObject> RootObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON in Spline Data file."));
		return;
	}

	const TArray<TSharedPtr<FJsonValue>>* PathsArray;
	if (RootObject->TryGetArrayField("Paths", PathsArray))
	{
		TArray<AActor*> FoundNodes;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATraversalNode::StaticClass(), FoundNodes);

		for (const TSharedPtr<FJsonValue>& PathValue : *PathsArray)
		{
			TSharedPtr<FJsonObject> PathObject = PathValue->AsObject();
			if (!PathObject.IsValid()) continue;

			FString NodeAName = PathObject->GetStringField("NodeA");
			FString NodeBName = PathObject->GetStringField("NodeB");

			// Find actual nodes in the world
			ATraversalNode* FoundNodeA = nullptr;
			ATraversalNode* FoundNodeB = nullptr;

			for (AActor* NodeActor : FoundNodes)
			{
				if (NodeActor->GetName() == NodeAName) FoundNodeA = Cast<ATraversalNode>(NodeActor);
				if (NodeActor->GetName() == NodeBName) FoundNodeB = Cast<ATraversalNode>(NodeActor);
			}

			if (FoundNodeA && FoundNodeB)
			{
				// Find their connection
				for (FWorldNeighbourStruct& Neighbour : FoundNodeA->NeighbouringNodes)
				{
					if (Neighbour.Node == FoundNodeB)
					{
						if (!IsValid(Neighbour.PathSplineComponent))
						{
							// If path was somehow deleted, recreate it first
							ActualPathCreation(FoundNodeA, &Neighbour);
						}

						if (IsValid(Neighbour.PathSplineComponent))
						{
							USplineComponent* Spline = Neighbour.PathSplineComponent;
							
							const TArray<TSharedPtr<FJsonValue>>* PointsArray;
							if (PathObject->TryGetArrayField("SplinePoints", PointsArray))
							{
								Spline->ClearSplinePoints();
								
								for (const TSharedPtr<FJsonValue>& PointValue : *PointsArray)
								{
									TSharedPtr<FJsonObject> PointObject = PointValue->AsObject();
									if (!PointObject.IsValid()) continue;

									TSharedPtr<FJsonObject> LocObj = PointObject->GetObjectField("Location");
									FVector Loc(LocObj->GetNumberField("X"), LocObj->GetNumberField("Y"), LocObj->GetNumberField("Z"));

									TSharedPtr<FJsonObject> TanObj = PointObject->GetObjectField("Tangent");
									FVector Tan(TanObj->GetNumberField("X"), TanObj->GetNumberField("Y"), TanObj->GetNumberField("Z"));

									Spline->AddSplinePoint(Loc, ESplineCoordinateSpace::Local, false);
									int32 LastIndex = Spline->GetNumberOfSplinePoints() - 1;
									Spline->SetTangentsAtSplinePoint(LastIndex, Tan, Tan, ESplineCoordinateSpace::Local, false);
								}
								Spline->UpdateSpline();
							}
						}
						break;
					}
				}
			}
		}

		UpdatePathMeshes();
		UE_LOG(LogTemp, Warning, TEXT("Successfully loaded Spline Data from %s"), *FilePath);
	}
#endif
}
