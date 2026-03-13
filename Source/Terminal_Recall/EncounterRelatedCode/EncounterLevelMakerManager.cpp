#include "EncounterLevelMakerManager.h"

#include "Terminal_Recall/GridCellClasses/GridVisualiser.h"

AEncounterLevelMakerManager::AEncounterLevelMakerManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEncounterLevelMakerManager::SpawnGrid()
{
	CreateGrid(FIntPoint(10, 10));
}

void AEncounterLevelMakerManager::CreateGrid(FIntPoint gridSize)
{
	if (!GridDisplayerObject)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GridDisplayerObject = GetWorld()->SpawnActor<AGridVisualiser>(GridDisplayerClass, FVector(50.0f, 50.0f, 0), FRotator::ZeroRotator, SpawnParams);
	}

	GridDisplayerObject->RegenerateGrid(gridSize);
}
