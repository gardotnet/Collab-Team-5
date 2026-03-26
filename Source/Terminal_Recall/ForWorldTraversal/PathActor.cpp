#include "PathActor.h"

#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/GridCellClasses/HarryWorldTraversalSystem.h"

APathActor::APathActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bRunConstructionScriptOnDrag = true; 
}

void APathActor::LinkStruct(AHarryWorldTraversalSystem* system, FHarryNeighbourStruct* relatedStruct)
{
	UE_LOG(LogTemp, Warning, TEXT("Linkine"));
	Modify();
	WorldTraversalSystem = system;
	RelatedStruct = relatedStruct;
}

void APathActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UE_LOG(LogTemp, Warning, TEXT("Im dying"));
	if (!IsValid(WorldTraversalSystem))
		WorldTraversalSystem = Cast<AHarryWorldTraversalSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), AHarryWorldTraversalSystem::StaticClass()));
	WorldTraversalSystem->RebuildMeshesForPath(RelatedStruct);
}
