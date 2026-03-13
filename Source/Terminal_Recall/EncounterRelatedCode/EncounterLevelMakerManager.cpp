#if WITH_EDITOR
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
	if (!GridDisplayerObject && GridDisplayerClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ran this"))
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (GetWorld()->IsGameWorld())
		{
			GridDisplayerObject = GetWorld()->SpawnActor<AGridVisualiser>(GridDisplayerClass, FVector(50.0f, 50.0f, 0), FRotator::ZeroRotator, SpawnParams);
		}
		else
		{
			// [NOTE] need to remove
			GridDisplayerObject = GEditor->GetEditorWorldContext().World()->SpawnActor<AGridVisualiser>(GridDisplayerClass, FVector(50.0f, 50.0f, 0), FRotator::ZeroRotator, SpawnParams);
		}
	}

	if (GridDisplayerObject)
		GridDisplayerObject->RegenerateGrid(gridSize);
}
#endif