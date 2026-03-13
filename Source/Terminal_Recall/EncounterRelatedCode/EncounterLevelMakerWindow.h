#pragma once

#include "CoreMinimal.h"
#include "Terminal_Recall/StructsAndEnums/LevelEditorBrushEnum.h"
#include "Terminal_Recall/StructsAndEnums/For2dArray/LevelCellData.h"

// So we can build the game, using "Blutility", "UMGEditor" will not build properly 
#if WITH_EDITOR
#include "EditorUtilityWidget.h"
#endif
// end it so the compiler doesn't say where is it lol
#include "EncounterLevelMakerWindow.generated.h"

#if WITH_EDITOR

class AGridVisualiser;
class AEncounterLevelMakerManager;
class UEditorUtilityComboBoxString;
class UEditorUtilityEditableTextBox;
class UEditorUtilityButton;


UCLASS()
class TERMINAL_RECALL_API UEncounterLevelMakerWindow : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
	// necessary to run during editor 
	#if WITH_EDITORONLY_DATA

	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	TSubclassOf<AGridVisualiser> VisualiserClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "For Designers | Necessities")
	TObjectPtr<UDataTable> EnemyDataTable;

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

	bool PlayerPlaced = false;
	FIntPoint PlayerSpawnPos = FIntPoint(-1, -1);
	
	TMap<FIntPoint, FLevelCellData> LevelGridData;
	
	#pragma region Buttons
	
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* CreateGridButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* WalkToggleButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* WallToggleButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* EnemyToggleButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* TrapToggleButton;
	UPROPERTY(meta = (BindWidget))
	UEditorUtilityButton* PlayerToggleButton;
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
	
	#pragma endregion 
	
	#endif
	
	virtual void NativeConstruct() override;
	
protected:
	
	#pragma region Link Functions
	
	UFUNCTION()
	void OnCreateGridPressed();
	UFUNCTION()
	void OnWalkTogglePressed();
	UFUNCTION()
	void OnWallTogglePressed();
	UFUNCTION()
	void OnEnemyTogglePressed();
	UFUNCTION()
	void OnTrapTogglePressed();
	UFUNCTION()
	void OnPlayerTogglePressed();
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
	
	#pragma endregion
	
	void UnselectCurrent();
	void HighlightComboBox(UEditorUtilityComboBoxString* comboBox);
	void UnhighlightComboBox(UEditorUtilityComboBoxString* comboBox);
	void SetUpTrack();
	void EnableMouseTrack();
	void ResetAndPopulateGridArray();
	TArray<float> GetColour(ECellBrushMode type);
};

#endif