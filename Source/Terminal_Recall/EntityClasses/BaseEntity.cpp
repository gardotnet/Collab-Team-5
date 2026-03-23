#include "BaseEntity.h"

#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/Controllers/DefaultPlayerController.h"

ABaseEntity::ABaseEntity()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RootComponent"));
	RootComponent = CapsuleComponent;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TheMesh"));
	MeshComponent->SetupAttachment(RootComponent);
}

void ABaseEntity::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	OnClicked.AddDynamic(this, &ABaseEntity::OnEntityClicked);
	TimerManagerRef = &GetWorld()->GetTimerManager();
	PlayerController = Cast<ADefaultPlayerController>(GetWorld()->GetFirstPlayerController());
	bIsMovingAlongSpline = false;
}

void ABaseEntity::StartMove(FVector position)
{
	bIsMovingAlongSpline = false;
	MoveTimer = 0;
	TargetPosition = position;
	TargetPosition.Z = GetActorLocation().Z;
	float distance = FVector::Dist(GetActorLocation(), position);
	DesiredMoveDuration = distance/Speed;
	TimerManagerRef->ClearTimer(MoveTimerHandle);
	TimerManagerRef->SetTimer(MoveTimerHandle, this, &ABaseEntity::OnMove, 0.01f, true);
}

void ABaseEntity::StartMoveAlongSpline(USplineComponent* SplinePath, bool bForward)
{
	if (!SplinePath) return;

	CurrentPathSpline = SplinePath;
	bIsMovingAlongSpline = true;
	bMovingForwardOnSpline = bForward;
	MoveTimer = 0;
	
	float SplineLength = SplinePath->GetSplineLength();
	DesiredMoveDuration = SplineLength / Speed;

	TimerManagerRef->ClearTimer(MoveTimerHandle);
	TimerManagerRef->SetTimer(MoveTimerHandle, this, &ABaseEntity::OnMove, 0.01f, true);
}

void ABaseEntity::OnMove()
{
	MoveTimer += 0.01f;

	if (bIsMovingAlongSpline && CurrentPathSpline)
	{
		float SplineLength = CurrentPathSpline->GetSplineLength();
		float Progress = MoveTimer / DesiredMoveDuration;

		if (Progress >= 1.0f)
		{
			// Reached destination
			float FinalDistance = bMovingForwardOnSpline ? SplineLength : 0.0f;
			FVector FinalLocation = CurrentPathSpline->GetLocationAtDistanceAlongSpline(FinalDistance, ESplineCoordinateSpace::World);
			SetActorLocation(FinalLocation);

			bIsMovingAlongSpline = false;
			CurrentPathSpline = nullptr;
			TimerManagerRef->ClearTimer(MoveTimerHandle);
			
			if (PlayerController)
				PlayerController->MovePlayer();
		}
		else
		{
			float CurrentDistance = bMovingForwardOnSpline ? 
				FMath::Lerp(0.0f, SplineLength, Progress) : 
				FMath::Lerp(SplineLength, 0.0f, Progress);

			FVector NewLocation = CurrentPathSpline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
			FRotator NewRotation = CurrentPathSpline->GetRotationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
			
			// If moving backwards, flip the rotation so the entity faces the direction it is moving
			if (!bMovingForwardOnSpline)
			{
				NewRotation.Yaw += 180.0f;
			}
			
			SetActorLocationAndRotation(NewLocation, NewRotation);
		}
	}
	else
	{
		// Old straight-line movement fallback
		FVector playerPos = GetActorLocation();
		float distance = FVector::Dist(playerPos, TargetPosition);
		
		if (MoveTimer > DesiredMoveDuration || distance < 1)
		{
			SetActorLocation(TargetPosition);
			TimerManagerRef->ClearTimer(MoveTimerHandle);
			if (PlayerController)
				PlayerController->MovePlayer();
		}
		else
		{
			SetActorLocation(FMath::Lerp(playerPos, TargetPosition, MoveTimer/DesiredMoveDuration));
		}
	}
}

void ABaseEntity::OnEntityClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	UE_LOG(LogTemp, Warning, TEXT("How dare you click me, I'm telling the world manager!"));
}
