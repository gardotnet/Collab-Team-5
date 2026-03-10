#include "WorldGridCell.h"

#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/Controllers/DefaultPlayerController.h"

AWorldGridCell::AWorldGridCell()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWorldGridCell::LoadLevel()
{
	// [NOTE] Will tell encounter "level loader" to load the level assign to this cell.
}

void AWorldGridCell::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<ADefaultPlayerController>(GetWorld()->GetFirstPlayerController());
}

void AWorldGridCell::OnCellClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	// [NOTE] Tell player controller to target this cell
	if (PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller ready and passing cell"));
		//PlayerController->NavigateWorld(this);
		
	}
}

