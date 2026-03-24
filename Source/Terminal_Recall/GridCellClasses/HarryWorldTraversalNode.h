#pragma once

#include "CoreMinimal.h"
#include "BaseGridCell.h"
#include "Terminal_Recall/StructsAndEnums/HarryNeighbourStruct.h"
#include "HarryWorldTraversalNode.generated.h"

UCLASS()
class TERMINAL_RECALL_API AHarryWorldTraversalNode : public ABaseGridCell
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal Node Info")
	TArray<FHarryNeighbourStruct> NeighbouringNodes;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Traversal Node Info")
	bool Visited = false;
	
	UPROPERTY(EditInstanceOnly, Category="Traversal Node Info", meta = (ToolTip = "Enable if node will trigger an encounter"))
	bool IsEncounter = true;
	
	UPROPERTY(EditInstanceOnly, Category="Traversal Node Info", meta = (ToolTip = "Paste the level's text file path here"))
	FString LevelFilePath;

protected:
	TArray<FHarryNeighbourStruct> OldNeighbouringNodes;
	
public:
	AHarryWorldTraversalNode();

	
};
