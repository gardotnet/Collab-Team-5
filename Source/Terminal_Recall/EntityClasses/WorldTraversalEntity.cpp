#include "WorldTraversalEntity.h"

#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/Controllers/DefaultPlayerController.h"
#include "Terminal_Recall/GridCellClasses/WorldGridCell.h"

AWorldTraversalEntity::AWorldTraversalEntity()
{
	PrimaryActorTick.bCanEverTick = true;
}
void AWorldTraversalEntity::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<ADefaultPlayerController>(GetWorld()->GetFirstPlayerController());
}
void AWorldTraversalEntity::OnMove()
{
	// to re-adjust the lerp 
	MoveTimer += 0.01f;
	FVector playerPos = GetActorLocation();
	float distance = FVector::Dist(playerPos, TargetPosition);
	// if reached target
	if (MoveTimer > DesiredMoveDuration || distance < 1)
	{
		// ensure it is at target
		SetActorLocation(TargetPosition);
		PlayerController->UpdatePlayerPos(TheCellPath[PathIndex]);
		PathIndex++;
		// if end point of navigation
		if (PathIndex >= TheCellPath.Num())
		{
			TheCellPath.Last()->OnUnitArrive_Implementation();
			TimerManagerRef->ClearTimer(MoveTimerHandle);
			return;
		}
		// move on to next point
		MoveTimer = 0;
		TargetPosition = TheCellPath[PathIndex]->GetActorLocation();
		TargetPosition.Z = GetActorLocation().Z;
		float newDistance = FVector::Dist(GetActorLocation(), TargetPosition);
		DesiredMoveDuration = newDistance/Speed;
	}
	else
	{
		SetActorLocation(FMath::Lerp(playerPos, TargetPosition, MoveTimer/DesiredMoveDuration));
	}
}
/// Set up for target destination, lerp calc and starts the movement
/// @param path Array of cells where it will travel to one by one
void AWorldTraversalEntity::StartMove(TArray<TObjectPtr<AWorldGridCell>> path)
{
	PathIndex = 0;
	MoveTimer = 0;
	TheCellPath = path;
	TargetPosition = path[0]->GetActorLocation();
	TargetPosition.Z = GetActorLocation().Z;
	float distance = FVector::Dist(GetActorLocation(), TargetPosition);
	DesiredMoveDuration = distance/Speed;
	TimerManagerRef->ClearTimer(MoveTimerHandle);
	TimerManagerRef->SetTimer(MoveTimerHandle, this, &AWorldTraversalEntity::OnMove, 0.01f, true);
}
