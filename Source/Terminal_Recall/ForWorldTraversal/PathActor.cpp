#include "PathActor.h"

#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/GridCellClasses/HarryWorldTraversalNode.h"
#include "Terminal_Recall/GridCellClasses/HarryWorldTraversalSystem.h"

APathActor::APathActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bRunConstructionScriptOnDrag = true; 
}

void APathActor::LinkStruct(AHarryWorldTraversalSystem*system, AHarryWorldTraversalNode* owningNode, FHarryNeighbourStruct relatedStruct)
{
	UE_LOG(LogTemp, Warning, TEXT("Linkine"));
	Modify();
	WorldTraversalSystem = system;
	Node = owningNode;
	RelatedStruct = relatedStruct;
}

void APathActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (Node != RelatedStruct.Node)
	{
		FHarryNeighbourStruct* found = Node->NeighbouringNodes.FindByPredicate([this](FHarryNeighbourStruct& actor)
		{
			return actor.PathObject == this;
		});
		
		if (found)
		{
			RelatedStruct = *found;
		}
	}
	
	if (WorldTraversalSystem.IsValid() && Node.IsValid())
		WorldTraversalSystem->RebuildMeshesForPath(&RelatedStruct);
}
