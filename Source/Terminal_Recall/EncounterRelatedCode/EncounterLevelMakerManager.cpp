#if WITH_EDITOR
#include "EncounterLevelMakerManager.h"

#include "Terminal_Recall/GridCellClasses/GridVisualiser.h"

AEncounterLevelMakerManager::AEncounterLevelMakerManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEncounterLevelMakerManager::ResetGrid()
{
	CreateGrid(GridSize);
}

void AEncounterLevelMakerManager::CreateGrid(FIntPoint gridSize)
{
	GridSize = FIntPoint(gridSize.X, gridSize.Y);
	if (!GridDisplayerObject && GridDisplayerClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (GetWorld()->IsGameWorld())
		{
			GridDisplayerObject = GetWorld()->SpawnActor<AGridVisualiser>(GridDisplayerClass, FVector(50.0f, 50.0f, 0), FRotator::ZeroRotator, SpawnParams);
		}
		else
		{
			GridDisplayerObject = GEditor->GetEditorWorldContext().World()->SpawnActor<AGridVisualiser>(GridDisplayerClass, FVector(50.0f, 50.0f, 0), FRotator::ZeroRotator, SpawnParams);
		}
	}

	if (GridDisplayerObject)
		GridDisplayerObject->RegenerateGrid(gridSize);
}

void AEncounterLevelMakerManager::Destroyed()
{
	Super::Destroyed();
	if (GridDisplayerObject)
		GridDisplayerObject->Destroy();
}
#endif
