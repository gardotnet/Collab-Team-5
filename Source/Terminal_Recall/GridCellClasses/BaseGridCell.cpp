#include "BaseGridCell.h"

#include "Components/BoxComponent.h"

ABaseGridCell::ABaseGridCell()
{
	PrimaryActorTick.bCanEverTick = false;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxRootPart"));
	RootComponent = BoxComponent;
	BoxComponent->SetCollisionProfileName("CellTraceProfile");
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TheMesh"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName("NoCollision");
}

void ABaseGridCell::BeginPlay()
{
	Super::BeginPlay();
	OnClicked.AddDynamic(this, &ABaseGridCell::OnCellClicked);
}

void ABaseGridCell::OnUnitArrive()
{
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
