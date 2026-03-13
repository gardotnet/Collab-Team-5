#include "GridVisualiser.h"

#include "Components/InstancedStaticMeshComponent.h"

AGridVisualiser::AGridVisualiser()
{
	PrimaryActorTick.bCanEverTick = true;
	
	USceneComponent* DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = DummyRoot;
	GridVisualComp = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Grid Visual");
	GridVisualComp->SetupAttachment(DummyRoot);
}

void AGridVisualiser::RegenerateGrid(FIntPoint gridSize)
{
	if (!GridVisualComp || !GridVisualComp->GetStaticMesh())
		return;
	
	GridVisualComp->ClearInstances();
	for (int x = 0; x < gridSize.X; x++)
	{
		for (int y = 0; y < gridSize.Y; y++)
		{
			FVector relativeLoc = FVector(x * TileSize, y * TileSize, 0.0f);
			FTransform instanceTransform = FTransform(relativeLoc);
			GridVisualComp->AddInstance(instanceTransform);

			FIntPoint gridIndex = FIntPoint(x, y);
			if (!GridData.Contains(gridIndex))
			{
				GridData.Add(gridIndex);
			}
			
		}
		
	}
}
