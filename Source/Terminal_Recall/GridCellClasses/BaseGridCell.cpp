#include "BaseGridCell.h"

ABaseGridCell::ABaseGridCell()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TheMesh"));
	MeshComponent->SetupAttachment(RootComponent);
}

void ABaseGridCell::BeginPlay()
{
	Super::BeginPlay();
	OnClicked.AddDynamic(this, &ABaseGridCell::OnCellClicked);
}

bool ABaseGridCell::OnUnitArrive(TObjectPtr<ABaseEntity> entityUnit)
{
	UnitRef = entityUnit;
	// [NOTE] bind 'OnUnitRefMoved' to entity's 'OnMoveEvent' 
	if (TrapRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ah ha this cell has been rigged!"));
		TrapRef->ActivateTrap();
	}
	return true;
}

void ABaseGridCell::OnUnitRefMoved()
{
	// [NOTE] Unbind 'OnUnitRefMoved' from entity's 'OnMoveEvent' 
	UnitRef = nullptr;
}

void ABaseGridCell::OnCellClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	if (UnitRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("This space is taken, pick another one."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Passing position to world manager."));
	}
}
