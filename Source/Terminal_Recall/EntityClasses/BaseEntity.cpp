#include "BaseEntity.h"

ABaseEntity::ABaseEntity()
{
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TheMesh"));
	MeshComponent->SetupAttachment(RootComponent);
}

void ABaseEntity::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHealth = MaxHealth;
	OnClicked.AddDynamic(this, &ABaseEntity::OnEntityClicked);
}

/*void ABaseEntity::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}*/

void ABaseEntity::OnMove(FVector3f pos)
{
	// [NOTE] move entity to passed position
}

void ABaseEntity::OnEntityClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	UE_LOG(LogTemp, Warning, TEXT("How dare you click me, I'm telling the world manager!"));
}



