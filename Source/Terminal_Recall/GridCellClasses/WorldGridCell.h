#pragma once

#include "CoreMinimal.h"
#include "BaseGridCell.h"
#include "Terminal_Recall/StructsAndEnums/WorldNeighbourStruct.h"
#include "WorldGridCell.generated.h"

UCLASS()
class TERMINAL_RECALL_API AWorldGridCell : public ABaseGridCell
{
	GENERATED_BODY()

public:
	AWorldGridCell();

	UPROPERTY(EditAnywhere, Category="Grid Cell Info (For Designers)", meta = (ToolTip = "Enable if cell will trigger an encounter"))
	bool IsEncounter = true;
	UPROPERTY(EditAnywhere, Category="Grid Cell Info (For Designers)", meta = (ToolTip = "Paste the level's text file path here"))
	FString LevelFilePath;
	UPROPERTY(EditAnywhere, Category="Grid Cell Info (For Designers)", meta = (ToolTip = "Make sure to add neighbours and it distance here!"))
	TArray<FWorldNeighbourStruct> NeighbouringCells;

	void LoadLevel();
	
	virtual void OnCellClicked(AActor* TouchedActor, FKey ButtonPressed) override;
	
	/*UPROPERTY(EditAnywhere, Category="Grid Cell Info (For Designers)", meta = (ToolTip = "Select the interactable related to this cell here"))
	TObjectPtr<AActor> InteractableRef; // [NOTE] will change to only Interactable Class instead of AActor*/
};
