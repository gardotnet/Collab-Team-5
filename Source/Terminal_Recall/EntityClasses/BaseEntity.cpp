#include "BaseEntity.h"

ABaseEntity::ABaseEntity()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseEntity::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHealth = MaxHealth;
}

/*void ABaseEntity::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}*/

void ABaseEntity::OnMove(FVector3f pos)
{
	// [NOTE] move entity to passed position
}



