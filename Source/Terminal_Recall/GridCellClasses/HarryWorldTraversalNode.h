#pragma once

#include "CoreMinimal.h"
#include "BaseGridCell.h"
#include "Components/SplineMeshComponent.h"
#include "Terminal_Recall/StructsAndEnums/HarryNeighbourStruct.h"
#include "Terminal_Recall/StructsAndEnums/ForDataTable/LevelnfoStruct.h"
#include "HarryWorldTraversalNode.generated.h"

class AHarryWorldTraversalSystem;
class ATraversalPlayerController;
class UDefaultInstance;

UCLASS()
class TERMINAL_RECALL_API AHarryWorldTraversalNode : public ABaseGridCell
{
	GENERATED_BODY()

public:
	AHarryWorldTraversalNode();
public:
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Grid Cell Info | Necessities", meta = (ToolTip = "Add the cell cover actor here"))
	TSubclassOf<AActor> CellCoverBpClass;
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Grid Cell Info | Necessities", meta = (ToolTip = "Add the cell cover material here"))
	TObjectPtr<UMaterial> CoverMaterial;
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Grid Cell Info | Necessities")
	TObjectPtr<UDataTable> LevelDataTable;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stuff For Designers | Grid Cell Info | General Stuff",  meta = (EditCondition = "VariableLoaded"))
	TArray<FHarryNeighbourStruct> NeighbouringNodes;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Stuff For Designers | Grid Cell Info | General Stuff")
	bool Visited = false;

	UPROPERTY(EditInstanceOnly, Category="Stuff For Designers | Grid Cell Info | Encounter Stuff", meta = (ToolTip = "Add the Level ID from \"Level Data Table\" here"))
	bool IsEncounter = true;
	UPROPERTY(EditInstanceOnly, Category="Stuff For Designers | Grid Cell Info | Encounter Stuff", meta = (ToolTip = "Add the Level ID from \"Level Data Table\" here"))
	bool IsOutpost= false; // for eventing endgame cutscenes
	UPROPERTY(EditInstanceOnly, Category="Stuff For Designers | Grid Cell Info | Encounter Stuff", meta = (ToolTip = "Add the Level ID from \"Level Data Table\" here"))
	int32 LevelID = 1;
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
protected:
	UPROPERTY()
	TObjectPtr<ATraversalPlayerController> PlayerController;
	UPROPERTY()
	TObjectPtr<UDefaultInstance> GameInstance;
	UPROPERTY(Transient)
	TArray<FHarryNeighbourStruct> OldNeighbouringNodes;
	
	UPROPERTY()
	TObjectPtr<AHarryWorldTraversalSystem> WorldTraversalSystem;
	UPROPERTY(VisibleInstanceOnly, Transient, Category="Stuff For Designers | Grid Cell Info | General Stuff")
	bool VariableLoaded = false;

public:
	virtual void BeginPlay() override;
	virtual void OnCellClicked(AActor* TouchedActor, FKey ButtonPressed) override;
	virtual void OnUnitArrive_Implementation() override;
	void CreateSinglePath(FHarryNeighbourStruct* neighbour);

protected:
	
#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostActorCreated() override;
	void InitializeVariables();
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
};
