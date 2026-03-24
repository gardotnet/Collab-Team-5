#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Terminal_Recall/GridCellClasses/GridVisualiser.h"
#include "Terminal_Recall/StructsAndEnums/GridRangeExport.h"
#include "GridRangeMakerWindow.generated.h"

class UEditorUtilityCheckBox;
class UTextBlock;
class UCanvasPanel;
class FLevelEditorModule;
class SLevelViewport;
class UEditorUtilityEditableTextBox;
class UEditorUtilityButton;

UCLASS()
class TERMINALRECALL_EDITOR_API UGridRangeMakerWindow : public UEditorUtilityWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	TSubclassOf<AGridVisualiser> VisualiserClass;
	
	UPROPERTY()
	TObjectPtr<UWorld> World;
	UPROPERTY()
	TObjectPtr<AGridVisualiser> VisualiserObject;
	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> LevelGridVisualISM;
	EGridRangeBrushMode CurrentModeSelected = EGridRangeBrushMode::None;
	ECompassDirection CurrentDirSelected = ECompassDirection::None;
	FTimerHandle HoverTimerHandle;
	FTimerManager* TimerManager;
	UPROPERTY()
	FIntPoint GridSize;
	
	UPROPERTY()
	FIntPoint LastHoverPos = FIntPoint(-1, -1);
	UPROPERTY()
	int32 LastHoverIndex = -1;
	UPROPERTY()
	int32 LastClickIndex = -1;
	UPROPERTY()
	int32 LastSelectedIndex = -1;
	UPROPERTY()
	bool MouseDownForSelect = false;
	//UPROPERTY()
	//FEditorGridRangeCellState info;
	FEditorGridRangeCellState* infoPtr;
	int32* MagnitudeChanging;
	
	UPROPERTY()
	TMap<FIntPoint, FEditorGridRangeCellState> LevelGridData;
	
	UPROPERTY()
	bool CentrePlaced = false;
	UPROPERTY()
	FIntPoint CentrePoint = FIntPoint(-1, -1);
	
	FString PrevXSizeText = "N/A"; 
	FString PrevYSizeText = "N/A"; 
	FString PrevMagText = "N/A"; 
	FString PrevGroupText = "N/A"; 
	
	#pragma region Buttons
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* CreateGridButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* EraseButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* SlotDrawButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* CentreButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* NorthButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* NorthEastButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* EastButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* SouthEastButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* SouthButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* SouthWestButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* WestButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* NorthWestButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* LoadLevelFileButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* ExportButton;
	/*UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* UndoButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* RedoButton;*/
	#pragma endregion
	#pragma region Editable Text Boxes
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityEditableTextBox* InputGridSizeX;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityEditableTextBox* InputGridSizeY;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityEditableTextBox* InputDirMag;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityEditableTextBox* InputGroupId;
	#pragma endregion

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* DirectionCanvas;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CellNameLabel;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DirLabel;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* GroupLabel;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityCheckBox* OneGroup;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	#pragma region Link Functions
	UFUNCTION()
	void OnCreateGrid();
	UFUNCTION()
	void OnEraseButtonClicked();
	UFUNCTION()
	void OnPaintButtonClicked();
	UFUNCTION()
	void SetCentrePoint();
	UFUNCTION()
	void OnNorthButtonClicked();
	UFUNCTION()
	void OnNorthEastButtonClicked();
	UFUNCTION()
	void OnEastButtonClicked();
	UFUNCTION()
	void OnSouthEastButtonClicked();
	UFUNCTION()
	void OnSouthButtonClicked();
	UFUNCTION()
	void OnSouthWestButtonClicked();
	UFUNCTION()
	void OnWestButtonClicked();
	UFUNCTION()
	void OnNorthWestButtonClicked();
	UFUNCTION()
	void OnLoadLevelFileButtonClicked();
	UFUNCTION()
	void OnExportButtonClicked();

	
	UFUNCTION()
	void OnGridTextXChange(const FText& NewText);
	UFUNCTION()
	void OnGridTextYChange(const FText& NewText);
	UFUNCTION()
	void OnDirMagChange(const FText& NewText);
	UFUNCTION()
	void OnGroupChange(const FText& NewText);
	
	UFUNCTION()
	void OnCheckOneGroup(bool isChecked);
	#pragma endregion
	#pragma region Utilities
	UFUNCTION()
	void TrackMouse();
	void ResetAndPopulateGridArray();
	void UnselectCurrentButton();
	void UnselectDirectionButton();
	inline void HideInputsForCell();
	void LoadDirectionsOnCell(FIntPoint pos);
	void SelectDirectionsButton(UEditorUtilityButton* button, ECompassDirection dir);
	void RefreshInputForGroup();
	int32* GetMagnitudeOfDirection();
	inline void UnselectCell();
	TArray<float> GetColour(EGridRangeBrushMode cellType);
	#pragma endregion
};
