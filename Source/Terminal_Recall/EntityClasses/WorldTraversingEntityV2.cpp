#include "WorldTraversingEntityV2.h"

#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/Controllers/TraversalPlayerController.h"
#include "Terminal_Recall/GridCellClasses/HarryWorldTraversalNode.h"
#include "Terminal_Recall/Harry/TraversalFunctionLibrary.h"

AWorldTraversingEntityV2::AWorldTraversingEntityV2()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWorldTraversingEntityV2::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<ATraversalPlayerController>(GetWorld()->GetFirstPlayerController());
}

void AWorldTraversingEntityV2::OnMove()
{
	MoveTimer += 0.01f;
	FVector playerPos = GetActorLocation();
	SetActorLocation(FMath::Lerp(playerPos, TargetPosition, MoveTimer/DesiredMoveDuration));
	
	float distance = FVector::Dist(playerPos, TargetPosition);
	// if reached target
	if (MoveTimer > DesiredMoveDuration || distance < 1)
	{
		// ensure it is at target
		SetActorLocation(TargetPosition);
		PlayerController->UpdatePlayerPos(TheNodePath[PathIndex]);
		// if end point of navigation
		if (PathIndex + 1 >= TheNodePath.Num())
		{
			TheNodePath.Last()->OnUnitArrive_Implementation();
			TimerManagerRef->ClearTimer(MoveTimerHandle);
			return;
		}
		TimerManagerRef->ClearTimer(MoveTimerHandle);
		MoveTimer = 0;
		
		// move on to next spline point
		auto* connectedNeighbour = TheNodePath[PathIndex]->NeighbouringNodes.FindByPredicate([&](const auto& info) {
			return info.Node == TheNodePath[PathIndex + 1];
		});
		UE_LOG(LogTemp, Log, TEXT("Next: %s and current %s"), *TheNodePath[PathIndex + 1]->GetActorNameOrLabel(), *TheNodePath[PathIndex]->GetActorNameOrLabel());
		CurrentSplineComp = connectedNeighbour->PathSplineComponent;
		bDontReverseSplineLoc = true;
		UTraversalFunctionLibrary::EvaluateSplineMovement(CurrentSplineComp, 0, bDontReverseSplineLoc, TargetPosition, TargetRotation);
		
		float DistanceToCurrent = FVector::DistSquared(TargetPosition, TheNodePath[PathIndex]->GetActorLocation());

		if (DistanceToCurrent > 0) 
		{
			bDontReverseSplineLoc = false; 
			UTraversalFunctionLibrary::EvaluateSplineMovement(CurrentSplineComp, 0.0f, bDontReverseSplineLoc, TargetPosition, TargetRotation);
		}
		UTraversalFunctionLibrary::CalculateSplineMoveDuration(CurrentSplineComp, Speed, DesiredMoveDuration);
		TargetPosition.Z = GetActorLocation().Z;
		TimerManagerRef->SetTimer(MoveTimerHandle, this, &AWorldTraversingEntityV2::OnSplineTraversal, 0.01f, true);
	}
}

void AWorldTraversingEntityV2::OnSplineTraversal()
{
	MoveTimer += 0.01f;
	// move on to next Spline point
	float splineCompletePer = FMath::Clamp(MoveTimer / DesiredMoveDuration, 0.0f, 1.0f);
	UTraversalFunctionLibrary::EvaluateSplineMovement(CurrentSplineComp, splineCompletePer , bDontReverseSplineLoc, TargetPosition, TargetRotation);
	TargetPosition.Z = GetActorLocation().Z;
	SetActorLocation(FMath::Lerp(GetActorLocation(), TargetPosition, MoveTimer/DesiredMoveDuration));
	if (splineCompletePer >= 1)
	{
		// move onto next path node
		TimerManagerRef->ClearTimer(MoveTimerHandle);
		PathIndex++;
		TargetPosition = TheNodePath[PathIndex]->GetActorLocation();
		TargetPosition.Z = GetActorLocation().Z;
		float newDistance = FVector::Dist(GetActorLocation(), TargetPosition);
		DesiredMoveDuration = newDistance/Speed;
		TimerManagerRef->SetTimer(MoveTimerHandle, this, &AWorldTraversingEntityV2::OnMove, 0.01f, true);
	}
}

void AWorldTraversingEntityV2::StartMove(TArray<TObjectPtr<AHarryWorldTraversalNode>> path)
{
	PathIndex = 0;
	MoveTimer = 0;
	TheNodePath = path;
	TargetPosition = path[0]->GetActorLocation();
	TargetPosition.Z = GetActorLocation().Z;
	float distance = FVector::Dist(GetActorLocation(), TargetPosition);
	DesiredMoveDuration = distance/Speed;
	TimerManagerRef->ClearTimer(MoveTimerHandle);
	TimerManagerRef->SetTimer(MoveTimerHandle, this, &AWorldTraversingEntityV2::OnMove, 0.01f, true);
}
