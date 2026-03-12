#include "PathReseter.h"

#include "WorldGridCell.h"
#include "Kismet/GameplayStatics.h"

APathReseter::APathReseter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APathReseter::ResetPositionForAllPath()
{
	TArray<AActor*> FoundCells;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWorldGridCell::StaticClass(), FoundCells);
	
	for (auto cell : FoundCells)
	{
		Cast<AWorldGridCell>(cell)->ResetPathPosition();
	}
	
}


