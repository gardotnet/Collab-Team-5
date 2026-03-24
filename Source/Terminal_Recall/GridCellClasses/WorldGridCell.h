#pragma once

#include "CoreMinimal.h"
#include "BaseGridCell.h"
#include "Terminal_Recall/StructsAndEnums/WorldNeighbourStruct.h"
#include "WorldGridCell.generated.h"

class ADefaultPlayerController;

UCLASS()
class TERMINAL_RECALL_API AWorldGridCell : public ABaseGridCell
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<ADefaultPlayerController> PlayerController;
	TArray<FWorldNeighbourStruct> OldNeighbouringCells;
public:
	AWorldGridCell();
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Grid Cell Info | Necessities", meta = (ToolTip = "Add the path actor here"))
	TSubclassOf<AActor> PathBpClass;
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Grid Cell Info | Necessities", meta = (ToolTip = "Add the cell cover actor here"))
	TSubclassOf<AActor> CellCoverBpClass;
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Grid Cell Info | Necessities", meta = (ToolTip = "Add the cell cover material here"))
	TObjectPtr<UMaterial> CoverMaterial;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Stuff For Designers | Grid Cell Info | General Stuff", meta = (ToolTip = "Make sure to add neighbours and it distance here!"))
	bool Visited = false;
	UPROPERTY(EditInstanceOnly, Category="Stuff For Designers | Grid Cell Info | General Stuff", meta = (ToolTip = "Make sure to add neighbours and it distance here!"))
	TArray<FWorldNeighbourStruct> NeighbouringCells;
	UPROPERTY(EditInstanceOnly, Category="Stuff For Designers | Grid Cell Info | Encounter Stuff", meta = (ToolTip = "Enable if cell will trigger an encounter"))
	bool IsEncounter = true;
	UPROPERTY(EditInstanceOnly, Category="Stuff For Designers | Grid Cell Info | Encounter Stuff", meta = (ToolTip = "Paste the level's text file path here"))
	FString LevelFilePath;
	UPROPERTY(EditInstanceOnly, Category="Stuff For Designers | Grid Cell Info | Cell Hider Options", meta = (ToolTip = "Enable if cell will be hidden"))
	bool IsCellHidden = false;
	UPROPERTY(EditInstanceOnly, Category="Stuff For Designers | Grid Cell Info | Cell Hider Options", meta = (ToolTip = "Set offset here", EditCondition = "IsCellHidden"))
	FVector CoverOffset = FVector(0,0, 0);
	UPROPERTY(EditInstanceOnly, Category="Stuff For Designers | Grid Cell Info | Cell Hider Options", meta = (ToolTip = "Set size here", EditCondition = "IsCellHidden"))
	FVector CoverSize = FVector(1,1, 1);
	UPROPERTY(EditInstanceOnly, Category="Stuff For Designers | Grid Cell Info | Cell Hider Options", meta = (ToolTip = "Set how far up it is from the cell here", EditCondition = "IsCellHidden"))
	float CoverHeight = 500;
	UPROPERTY()
	TObjectPtr<AActor> CoverObject;
public:
	UFUNCTION(BlueprintCallable,  Category="Stuff For Designers | Functions")
	void LoadLevel();
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Stuff For Designers | Grid Cell Tool")
	void CreatePath();
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Stuff For Designers | Grid Cell Tool")
	void ResetPathPosition();
	virtual void OnCellClicked(AActor* TouchedActor, FKey ButtonPressed) override;
	virtual void OnUnitArrive_Implementation() override;
protected:
	void ActualPathCreation(FWorldNeighbourStruct* neighbour);
	virtual void BeginPlay() override;
#if WITH_EDITOR
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
};
