#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Terminal_Recall/GridCellClasses/GridVisualiser.h"
#include "GridRangeMakerWindow.generated.h"

class FLevelEditorModule;
class SLevelViewport;
class UEditorUtilityEditableTextBox;
class UEditorUtilityButton;

UENUM()
enum class EGridRangeBrushMode : int32
{
	None,
	Erase,
	Slot,
	N,
	NE,
	E,
	SE,
	S,
	SW,
	W,
	NW
};

USTRUCT()
struct FGridRangeCellInfo
{
	GENERATED_BODY()
	
	UPROPERTY()
	bool IsPainted;
	UPROPERTY()
	TArray<EGridRangeBrushMode> Modifiers;
};

UCLASS()
class TERMINAL_RECALL_API UGridRangeMakerWindow : public UEditorUtilityWidget
{
	GENERATED_BODY()
#if WITH_EDITOR
	#if WITH_EDITORONLY_DATA
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
	FTimerHandle HoverTimerHandle;
	FTimerManager* TimerManager;
	FIntPoint GridSize;
	FIntPoint LastHoverPos = FIntPoint(-1, -1);
	int32 LastHoverIndex = -1;
	int32 LastClickIndex = -1;
	TMap<FIntPoint, FGridRangeCellInfo> LevelGridData;
	
	#pragma region Buttons
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* CreateGridButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* EraseButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* SlotDrawButton;
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
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityEditableTextBox* InputDirMag;
	#pragma endregion
	
	virtual void NativeConstruct() override;
	
	#pragma region Link Functions
	UFUNCTION()
	void OnCreateGrid();
	UFUNCTION()
	void OnEraseButtonClicked();
	UFUNCTION()
	void OnPaintButtonClicked();
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
	void OnUndoButtonClicked();
	UFUNCTION()
	void OnRedoButtonClicked();
	
	UFUNCTION()
	void OnGridTextXChange(const FText& NewText);
	UFUNCTION()
	void OnGridTextYChange(const FText& NewText);
	UFUNCTION()
	void OnDirMagChange(const FText& NewText);
	#pragma endregion
	
	void SetUpMouseTrack();
	UFUNCTION()
	void TrackMouse();
	
	void ResetAndPopulateGridArray();
	void UnselectCurrentButton();
	TArray<float> GetColour(bool isPainted);
	
	
	
	#endif
#endif
};
