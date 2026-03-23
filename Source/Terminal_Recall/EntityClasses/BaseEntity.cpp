#include "BaseEntity.h"

#include "Components/CapsuleComponent.h"

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
	TimerManagerRef = TimerManagerRef = &GetWorld()->GetTimerManager();
}
void ABaseEntity::StartMove(FVector position)
{
	MoveTimer = 0;
	TargetPosition = position;
	TargetPosition.Z = GetActorLocation().Z;
	float distance = FVector::Dist(GetActorLocation(), position);
	DesiredMoveDuration = distance/Speed;
	TimerManagerRef->ClearTimer(MoveTimerHandle);
	TimerManagerRef->SetTimer(MoveTimerHandle, this, &ABaseEntity::OnMove, 0.01f, true);
}

void ABaseEntity::StartMove(TArray<TObjectPtr<AWorldGridCell>> path)
{
}

void ABaseEntity::OnMove()
{
}

void ABaseEntity::OnEntityClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	UE_LOG(LogTemp, Warning, TEXT("How dare you click me, I'm telling the world manager!"));
}
/*void ABaseEntity::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}*/

