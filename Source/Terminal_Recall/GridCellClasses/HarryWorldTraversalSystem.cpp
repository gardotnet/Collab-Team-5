#include "HarryWorldTraversalSystem.h"

#include "HarryWorldTraversalNode.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/ForWorldTraversal/CustomSplineComp.h"
#include "Terminal_Recall/ForWorldTraversal/PathActor.h"
#include "Terminal_Recall/StructsAndEnums/HarryNeighbourStruct.h"

AHarryWorldTraversalSystem::AHarryWorldTraversalSystem()
{
	PrimaryActorTick.bCanEverTick = false;
	PathMeshForwardAxis = ESplineMeshAxis::X;
}

void AHarryWorldTraversalSystem::ResetPathPosition()
{
#if WITH_EDITOR
	TArray<AActor*> FoundNodes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHarryWorldTraversalNode::StaticClass(), FoundNodes);

	for (AActor* NodeActor : FoundNodes)
	{
		AHarryWorldTraversalNode* Node = Cast<AHarryWorldTraversalNode>(NodeActor);
		if (!IsValid(Node)) continue;

		for (FHarryNeighbourStruct& Neighbour : Node->NeighbouringNodes)
		{
			if (!IsValid(Neighbour.Node)) continue;

			FVector PosA = Node->GetActorLocation();
			FVector PosB = Neighbour.Node->GetActorLocation();
			PosB.Z = PosA.Z;

			float Dist = FVector::Dist(PosA, PosB);
			FVector MidPoint = (PosA + PosB) * 0.5f;

			if (IsValid(Neighbour.PathObject))
			{
				// We don't change rotation because splines handle the curve
				Neighbour.PathObject->SetActorLocation(MidPoint);
				
				if (IsValid(Neighbour.PathSplineComponent))
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

void AHarryWorldTraversalSystem::CreatePaths()
{
	#if WITH_EDITOR
	TArray<AActor*> FoundNodes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHarryWorldTraversalNode::StaticClass(), FoundNodes);
	
	for (AActor* NodeActor : FoundNodes)
	{
		AHarryWorldTraversalNode* Node = Cast<AHarryWorldTraversalNode>(NodeActor);
		if (!Node) continue;
		
		for (FHarryNeighbourStruct& Neighbour : Node->NeighbouringNodes)
		{
			if (IsValid(Neighbour.Node))
			{
				ActualPathCreation(Node, &Neighbour);
			}
		}
	}
	UpdatePathMeshes();
	#endif
}

void AHarryWorldTraversalSystem::UpdatePathMeshes()
{
#if WITH_EDITOR
	TArray<AActor*> FoundNodes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHarryWorldTraversalNode::StaticClass(), FoundNodes);

	for (AActor* NodeActor : FoundNodes)
	{
		AHarryWorldTraversalNode* Node = Cast<AHarryWorldTraversalNode>(NodeActor);
		if (!IsValid(Node)) continue;

		for (FHarryNeighbourStruct& Neighbour : Node->NeighbouringNodes)
		{
			if (IsValid(Neighbour.PathObject) && IsValid(Neighbour.PathSplineComponent))
			{
				RebuildMeshesForPath(&Neighbour);
			}
		}
	}
#endif
}

void AHarryWorldTraversalSystem::ClearPaths()
{
#if WITH_EDITOR
	TArray<AActor*> FoundNodes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHarryWorldTraversalNode::StaticClass(), FoundNodes);

	// First pass: clear all references from the nodes
	for (AActor* NodeActor : FoundNodes)
	{
		AHarryWorldTraversalNode* Node = Cast<AHarryWorldTraversalNode>(NodeActor);
		if (!IsValid(Node)) continue;

		for (int32 i = 0; i < Node->NeighbouringNodes.Num(); i++)
		{
			FHarryNeighbourStruct& Neighbour = Node->NeighbouringNodes[i];
			
			ClearSpawnedActors(&Neighbour);
			
			if (IsValid(Neighbour.PathObject))
			{
				Neighbour.PathObject->Destroy();
			}
			
			Neighbour.PathObject = nullptr;
			Neighbour.PathSplineComponent = nullptr;
			Neighbour.HasPathConnected = false;
		}
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

void AHarryWorldTraversalSystem::ActualPathCreation(AHarryWorldTraversalNode* NodeA,
                                                    FHarryNeighbourStruct* NeighbourInfo)
{
	#if WITH_EDITOR
	if (!IsValid(NodeA) || !NeighbourInfo || !IsValid(NeighbourInfo->Node)) return;

	NodeA->Modify();
	AHarryWorldTraversalNode* NodeB = NeighbourInfo->Node;
	NodeB->Modify();

	if (NeighbourInfo->HasPathConnected)
		return;

	NeighbourInfo->HasPathConnected = true;

	// Check if NodeB already has NodeA as a neighbour and connected
	FHarryNeighbourStruct* FoundSelf = NodeB->NeighbouringNodes.FindByPredicate([NodeA](const FHarryNeighbourStruct& Item)
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
		FHarryNeighbourStruct NewStruct;
		NewStruct.Node = NodeA;
		NodeB->NeighbouringNodes.Add(NewStruct);
		
		FoundSelf = NodeB->NeighbouringNodes.FindByPredicate([NodeA](const FHarryNeighbourStruct& Item)
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

	FRotator zeroRotation = FRotator(0.0f, 0.0f, 0.0f);
	
	if (APathActor* NewPath = GetWorld()->SpawnActor<APathActor>(SplinePathBpClass, SpawnPoint, zeroRotation, SpawnParams))
	{
		NewPath->Modify();
		NewPath->LinkStruct(this, NeighbourInfo);
		NewPath->Tags.Add(FName("TraversalPathActor"));
		UCustomSplineComp* SplineComp = Cast<UCustomSplineComp>(NewPath->GetComponentByClass(UCustomSplineComp::StaticClass()));
		if (!IsValid(SplineComp))
		{
			SplineComp = NewObject<UCustomSplineComp>(NewPath, UCustomSplineComp::StaticClass(), TEXT("PathSpline"));
			if (IsValid(SplineComp))
			{
				SplineComp->RegisterComponent();
				NewPath->AddInstanceComponent(SplineComp);
			}
		}
		SplineComp->Modify();
		if (IsValid(SplineComp))
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

void AHarryWorldTraversalSystem::RebuildMeshesForPath(FHarryNeighbourStruct* NeighbourInfo)
{
	#if WITH_EDITOR
	if (!NeighbourInfo || !IsValid(NeighbourInfo->PathObject) || !IsValid(NeighbourInfo->PathSplineComponent)) return;

	AActor* PathActor = NeighbourInfo->PathObject;
	UCustomSplineComp* Spline = NeighbourInfo->PathSplineComponent;

	PathActor->Modify();
	Spline->Modify();

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
	if (bSpawnActorsOnPath && IsValid(ActorToSpawnClass))
	{
		// Roll for a single spawn along the path
		float RandomRoll = FMath::RandRange(0.f, 100.f);
		if (RandomRoll <= SpawnChance)
		{
			// Pick a random distance along the spline
			float SplineLength = Spline->GetSplineLength();
			float RandomDistance = FMath::RandRange(0.f, SplineLength);

			// Calculate final transform based on the spline at this random distance
			FVector Location = Spline->GetLocationAtDistanceAlongSpline(RandomDistance, ESplineCoordinateSpace::World);
			FRotator Rotation = Spline->GetRotationAtDistanceAlongSpline(RandomDistance, ESplineCoordinateSpace::World);

			// Add custom offset (rotated by the spline's rotation at this point)
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
			}
		}
	}

	// Always generate meshes if mesh is set
	if (IsValid(PathMesh))
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
				if (IsValid(SplineMesh))
				{
					SplineMesh->Modify();
					SplineMesh->SetStaticMesh(PathMesh);
					SplineMesh->SetForwardAxis(PathMeshForwardAxis);
					SplineMesh->SetMobility(EComponentMobility::Movable); 
					
					SplineMesh->SetStartScale(PathMeshScale, false);
					SplineMesh->SetEndScale(PathMeshScale, false);
					
					SplineMesh->SetStartOffset(PathMeshOffset, false);
					SplineMesh->SetEndOffset(PathMeshOffset, false);
					
					SplineMesh->RegisterComponent();
					PathActor->AddInstanceComponent(SplineMesh);
					SplineMesh->AttachToComponent(Spline, FAttachmentTransformRules::KeepRelativeTransform);
					
					float SplineMaxLen = Spline->GetSplineLength();
					float SafeStart = FMath::Clamp(CurrentStartDist, 0.f, SplineMaxLen);
					float SafeEnd = FMath::Clamp(CurrentEndDist, 0.f, SplineMaxLen);

					FVector StartPos = Spline->GetLocationAtDistanceAlongSpline(SafeStart, ESplineCoordinateSpace::Local);
					FVector StartTangent = Spline->GetTangentAtDistanceAlongSpline(SafeStart, ESplineCoordinateSpace::Local);
					
					FVector EndPos = Spline->GetLocationAtDistanceAlongSpline(SafeEnd, ESplineCoordinateSpace::Local);
					FVector EndTangent = Spline->GetTangentAtDistanceAlongSpline(SafeEnd, ESplineCoordinateSpace::Local);
					
					StartTangent = StartTangent.GetSafeNormal() * SegmentLength;
					EndTangent = EndTangent.GetSafeNormal() * SegmentLength;

					SplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, true);
				}
			}
		}
	}
#endif
}

void AHarryWorldTraversalSystem::ClearSpawnedActors(FHarryNeighbourStruct* NeighbourInfo)
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
