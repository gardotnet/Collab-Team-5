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

	TArray<float> color = { 1.0f, 1.0f, 1.0f, 1.0f };
	
	GridVisualComp->ClearInstances();
	for (int x = 0; x < gridSize.X; x++)
	{
		for (int y = 0; y < gridSize.Y; y++)
		{
			FVector relativeLoc = FVector(x * TileSize, y * TileSize, 0.0f);
			FTransform instanceTransform = FTransform(relativeLoc);
			int32 instanceIndex =  GridVisualComp->AddInstance(instanceTransform);
			GridVisualComp->SetCustomData(instanceIndex, color, false);
		}
	}
}
