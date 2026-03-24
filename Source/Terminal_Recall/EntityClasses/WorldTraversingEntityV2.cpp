#include "WorldTraversingEntityV2.h"

#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/Controllers/DefaultPlayerController.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/Controllers/TraversalPlayerController.h"
#include "Terminal_Recall/GridCellClasses/HarryWorldTraversalNode.h"

AWorldTraversingEntityV2::AWorldTraversingEntityV2()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWorldTraversingEntityV2::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<ATraversalPlayerController>(GetWorld()->GetFirstPlayerController());
}

void AWorldTraversingEntityV2::StartMove(TArray<TObjectPtr<AHarryWorldTraversalNode>> path)
{
	MoveTimer += 0.01f;
	FVector playerPos = GetActorLocation();
	float distance = FVector::Dist(playerPos, TargetPosition);
	// if reached target
	if (MoveTimer > DesiredMoveDuration || distance < 1)
	{
		// ensure it is at target
		SetActorLocation(TargetPosition);
		PlayerController->UpdatePlayerPos(TheNodePath[PathIndex]);
		PathIndex++;
		// if end point of navigation
		if (PathIndex >= TheNodePath.Num())
		{
			TheNodePath.Last()->OnUnitArrive_Implementation();
			TimerManagerRef->ClearTimer(MoveTimerHandle);
			return;
		}
		// move on to next point
		MoveTimer = 0;
		TargetPosition = TheNodePath[PathIndex]->GetActorLocation();
		TargetPosition.Z = GetActorLocation().Z;
		float newDistance = FVector::Dist(GetActorLocation(), TargetPosition);
		DesiredMoveDuration = newDistance/Speed;
	}
	else
	{
		SetActorLocation(FMath::Lerp(playerPos, TargetPosition, MoveTimer/DesiredMoveDuration));
	}
}