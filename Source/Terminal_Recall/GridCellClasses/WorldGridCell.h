#pragma once

#include "CoreMinimal.h"
#include "BaseGridCell.h"
#include "Terminal_Recall/StructsAndEnums/WorldNeighbourStruct.h"
#include "Terminal_Recall/StructsAndEnums/ForDataTable/LevelnfoStruct.h"
#include "WorldGridCell.generated.h"

class UDefaultInstance;
class ADefaultPlayerController;

UCLASS()
class TERMINAL_RECALL_API AWorldGridCell : public ABaseGridCell
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<ADefaultPlayerController> PlayerController;
	TArray<FWorldNeighbourStruct> OldNeighbouringCells;
	UPROPERTY()
	TObjectPtr<UDefaultInstance> GameInstance;
public:
	AWorldGridCell();
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Grid Cell Info | Necessities", meta = (ToolTip = "Add the path actor here"))
	TSubclassOf<AActor> PathBpClass;
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Grid Cell Info | Necessities", meta = (ToolTip = "Add the cell cover actor here"))
	TSubclassOf<AActor> CellCoverBpClass;
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Grid Cell Info | Necessities", meta = (ToolTip = "Add the cell cover material here"))
	TObjectPtr<UMaterial> CoverMaterial;
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Grid Cell Info | Necessities")
	TObjectPtr<UDataTable> LevelDataTable;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stuff For Designers | Grid Cell Info | General Stuff")
	bool Visited = false;
	UPROPERTY(EditInstanceOnly, Category="Stuff For Designers | Grid Cell Info | General Stuff", meta = (ToolTip = "Make sure to add neighbours and it distance here!"))
	TArray<FWorldNeighbourStruct> NeighbouringCells;
	
	UPROPERTY(EditInstanceOnly, Category="Stuff For Designers | Grid Cell Info | Encounter Stuff", meta = (ToolTip = "Add the Level ID from \"Level Data Table\" here"))
	int32 LevelID = 0;
	UPROPERTY(BlueprintReadOnly, Category="Stuff For Designers | Grid Cell Info | Encounter Stuff")
	FLevelInfoStruct LevelInfo;
	
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
	
	bool NeighbourCloudRemoved = false;
public:
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
