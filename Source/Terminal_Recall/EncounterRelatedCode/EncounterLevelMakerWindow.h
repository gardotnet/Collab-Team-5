#pragma once

#include "CoreMinimal.h"
#include "Terminal_Recall/StructsAndEnums/LevelEditorBrushEnum.h"
#include "Terminal_Recall/StructsAndEnums/LevelEditorUndoStruct.h"
#include "Terminal_Recall/StructsAndEnums/For2dArray/LevelCellData.h"
#include "EditorUtilityWidget.h"
#include "EncounterLevelMakerWindow.generated.h"

class UTextRenderComponent;
struct FLevelExportStruct;
class AGridVisualiser;
class AEncounterLevelMakerManager;
class UEditorUtilityComboBoxString;
class UEditorUtilityEditableTextBox;
class UEditorUtilityButton;

USTRUCT(BlueprintType)
struct FGridPosAnd1DIndex
{
	GENERATED_BODY()
	UPROPERTY()
	FIntPoint GridPos = FIntPoint(-1);
	UPROPERTY()
	int32 IndexPos = -1;
};

UCLASS()
class TERMINAL_RECALL_API UEncounterLevelMakerWindow : public UEditorUtilityWidget
{
	GENERATED_BODY()

	// So we can build the game, using "Blutility", "UMGEditor" will not build properly 
#if WITH_EDITOR
	// necessary to run during editor 
	#if WITH_EDITORONLY_DATA

	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	TSubclassOf<AGridVisualiser> VisualiserClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "For Designers | Necessities")
	TObjectPtr<UDataTable> PropsDataTable;
	UPROPERTY(EditDefaultsOnly, Category = "For Designers | Necessities")
	TObjectPtr<UDataTable> EnemyDataTable;
	UPROPERTY(EditDefaultsOnly, Category = "For Designers | Necessities")
	TObjectPtr<UDataTable> TrapDataTable;
	UPROPERTY(EditDefaultsOnly, Category = "For Designers | Necessities")
	TObjectPtr<UDataTable> InteractableDataTable;

	UPROPERTY()
	TObjectPtr<UWorld> World;
	UPROPERTY()
	TObjectPtr<AGridVisualiser> VisualiserObject;
	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> LevelGridVisualISM;
	
	ECellBrushMode CurrentModeSelected = ECellBrushMode::None;
	FTimerHandle HoverTimerHandle;
	
	FTimerManager* TimerManager;
	FIntPoint GridSize;
	FIntPoint LastHoverPos = FIntPoint(-1, -1);
	int32 LastHoverIndex = -1;
	int32 LastClickIndex = -1;
	FIntPoint SelectSize = FIntPoint(-1);
	TArray<FGridPosAnd1DIndex> SelectedCells;
	bool Obstructed = false;
	int32 SelectedGroupMainCellIndex = -1;
	
	bool PlayerPlaced = false;
	FIntPoint PlayerSpawnPos = FIntPoint(-1, -1);
	
	TMap<FIntPoint, FLevelCellData> LevelGridData;
	
	TArray<FLevelEditorUndo> UndoList;
	bool CursorDown = false;
	FLevelEditorUndo CurrentUndoSession;
	int32 UndoIndex = 0;
	
	#pragma region Buttons
	
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* CreateGridButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* WalkToggleButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* WallToggleButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* PlayerToggleButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* EnemyToggleButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* TrapToggleButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* InteractableToggleButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* LoadLevelFileButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* ExportButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* UndoButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* RedoButton;
	
	#pragma endregion
	#pragma region Editable Text Boxes
	
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityEditableTextBox* InputGridSizeX;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityEditableTextBox* InputGridSizeY;
	
	#pragma endregion
	#pragma region Combo Boxes
	
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityComboBoxString* EnemyComboBox;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityComboBoxString* TrapComboBox;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityComboBoxString* InteractableComboBox;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityComboBoxString* PropsComboBox;
	
	#pragma endregion 
	
	#endif
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
protected:
	TSharedPtr<class FUndoKeyInput> MyInputProcessor;
	#pragma region Link Functions
	UFUNCTION()
	void OnCreateGridPressed();
	UFUNCTION()
	void OnWalkableTogglePressed();
	UFUNCTION()
	void OnWallTogglePressed();
	UFUNCTION()
	void OnPlayerTogglePressed();
	UFUNCTION()
	void OnEnemyTogglePressed();
	UFUNCTION()
	void OnTrapTogglePressed();
	UFUNCTION()
	void OnInteractableTogglePressed();
	UFUNCTION()
	void OnLoadFilePressed();
	UFUNCTION()
	void OnExportPressed();
	UFUNCTION()
	void OnUndoPressed();
	UFUNCTION()
	void OnRedoPressed();
	UFUNCTION()
	void OnGridTextXChange(const FText& NewText);
	UFUNCTION()
	void OnGridTextYChange(const FText& NewText);
	UFUNCTION()
	void OnEnemySelectChange(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnTrapSelectChange(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnInteractableSelectChange(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnPropSelectChange(FString SelectedItem, ESelectInfo::Type SelectionType);
	
	#pragma endregion
	
	#pragma region Utilities Functions
	void UnselectCurrentButton();
	void HighlightComboBox(UEditorUtilityComboBoxString* comboBox);
	void UnhighlightComboBox(UEditorUtilityComboBoxString* comboBox);
	void SetUpTrack();
	void EnableMouseTrack();
	void ResetAndPopulateGridArray();
	void ProcessExportData(FLevelExportStruct exportData);
	void UnhoverGridCell();
	void CalculateSelectCells(FIntPoint gridPos);
	void HandleNeighbouringCells(int32 placedIndex);
	int32 GetRelevantObjID(ECellBrushMode type) const;
	TArray<FGridPosAnd1DIndex> GetGroupedCells(FLevelCellData* selectedCellData);
	FLevelExportStruct ConvertToExportData();
	TArray<float> GetColour(ECellBrushMode type);
	TObjectPtr<AGridVisualiser> GetGridVisualiser();
	bool SetUpTextComp(const FLevelCellData* newData, const FLevelCellData* prevData, int32 ismIndex);
	FIntPoint GetGroupSize(ECellBrushMode type, int32 id = -1, bool isSelectMode = true);
	
	inline void AddUndoSession();
	
public:
	bool ActualUndo();
	bool ActualRedo();
	
	#pragma endregion
#endif
	
};

