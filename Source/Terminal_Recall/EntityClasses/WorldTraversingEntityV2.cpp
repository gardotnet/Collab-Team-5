#include "WorldTraversingEntityV2.h"

#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/DefaultInstance.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/Controllers/TraversalPlayerController.h"
#include "Terminal_Recall/GridCellClasses/HarryWorldTraversalNode.h"
#include "Terminal_Recall/Harry/TraversalFunctionLibrary.h"

AWorldTraversingEntityV2::AWorldTraversingEntityV2()
{
	PrimaryActorTick.bCanEverTick = true;

	MouseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MouseMesh"));
	MouseMesh->SetupAttachment(MeshComponent);
	
	//Mesh for mouse in cockpit, NOT freeroaming
	CharaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CharaMesh"));
	CharaMesh->SetupAttachment(MeshComponent);

	R_ArmMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightArmMesh"));
	R_ArmMesh->SetupAttachment(MeshComponent);

	L_ArmMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftArmMesh"));
	L_ArmMesh->SetupAttachment(MeshComponent);

	R_LegMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightLegMesh"));
	R_LegMesh->SetupAttachment(MeshComponent);

	L_LegMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftLegMesh"));
	L_LegMesh->SetupAttachment(MeshComponent);

	BoostersMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoostersMesh"));
	BoostersMesh->SetupAttachment(MeshComponent);

	CockpitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CockpitMesh"));
	CockpitMesh->SetupAttachment(MeshComponent);

	WindshieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WindshieldMesh"));
	WindshieldMesh->SetupAttachment(MeshComponent);

	//Hide mech parts on start:
	CharaMesh->SetHiddenInGame(true);
	R_ArmMesh->SetHiddenInGame(true);
	L_ArmMesh->SetHiddenInGame(true);
	R_LegMesh->SetHiddenInGame(true);
	L_LegMesh->SetHiddenInGame(true);
	BoostersMesh->SetHiddenInGame(true);
	CockpitMesh->SetHiddenInGame(true);
	WindshieldMesh->SetHiddenInGame(true);
	
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
			TheReversePath.Add(TheNodePath[PathIndex]);
			Algo::Reverse(TheReversePath);
			TheNodePath.Last()->OnUnitArrive_Implementation();
			TimerManagerRef->ClearTimer(MoveTimerHandle);
			return;
		}
		TimerManagerRef->ClearTimer(MoveTimerHandle);
		MoveTimer = 0;
		
		if (TheGameInstance->VisitedCells.Contains(TheNodePath[PathIndex]->GetActorLocation()))
		{
			TheReversePath.Empty();
			TheReversePath.Add(TheNodePath[PathIndex]);
		}
		else
		{
			TheReversePath.Add(TheNodePath[PathIndex]);
		}
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
	TheGameInstance = Cast<UDefaultInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
}

void AWorldTraversingEntityV2::MoveBack()
{
	StartMove(TheReversePath);
}

void AWorldTraversingEntityV2::EquipMechPart() {
	if (!TheGameInstance) return;
	
}

