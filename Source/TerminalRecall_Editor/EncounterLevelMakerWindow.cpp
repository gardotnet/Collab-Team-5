#include "EncounterLevelMakerWindow.h"
#include "DesktopPlatformModule.h"
#include "EditorUtilityWidgetComponents.h"
#include "IDesktopPlatform.h"
#include "JsonObjectConverter.h"
#include "LevelEditor.h"
#include "SLevelViewport.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Keybinds/InputProcessor/FUndoKeyInput.h"
#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/GridCellClasses/GridVisualiser.h"
#include "Terminal_Recall/StructsAndEnums/LevelExportStruct.h"
#include "Terminal_Recall/StructsAndEnums/ForDataTable/AssetLookUpStruct.h"
#include "Terminal_Recall/StructsAndEnums/ForDataTable/EnemyInfoStruct.h"
#include "Terminal_Recall/StructsAndEnums/ForDataTable/InteractableInfoStruct.h"
#include "Terminal_Recall/StructsAndEnums/ForDataTable/TrapInfoStruct.h"

void UEncounterLevelMakerWindow::NativeConstruct()
{
	Super::NativeConstruct();
	HoverTimerHandle = FTimerHandle();
	#pragma region UI Links
	if (CreateGridButton)
		CreateGridButton->OnClicked.AddDynamic(this, &UEncounterLevelMakerWindow::OnCreateGridPressed);
	if (WalkToggleButton)
		WalkToggleButton->OnClicked.AddDynamic(this, &UEncounterLevelMakerWindow::OnWalkableTogglePressed);
	if (WallToggleButton)
		WallToggleButton->OnClicked.AddDynamic(this, &UEncounterLevelMakerWindow::OnWallTogglePressed);
	if (PlayerToggleButton)
		PlayerToggleButton->OnClicked.AddDynamic(this, &UEncounterLevelMakerWindow::OnPlayerTogglePressed);
	if (EnemyToggleButton)
		EnemyToggleButton->OnClicked.AddDynamic(this, &UEncounterLevelMakerWindow::OnEnemyTogglePressed);
	if (TrapToggleButton)
		TrapToggleButton->OnClicked.AddDynamic(this, &UEncounterLevelMakerWindow::OnTrapTogglePressed);
	if (InteractableToggleButton)
		InteractableToggleButton->OnClicked.AddDynamic(this, &UEncounterLevelMakerWindow::OnInteractableTogglePressed);
	if (LoadLevelFileButton)
		LoadLevelFileButton->OnClicked.AddDynamic(this, &UEncounterLevelMakerWindow::OnLoadFilePressed);
	if (ExportButton)
		ExportButton->OnClicked.AddDynamic(this, &UEncounterLevelMakerWindow::OnExportPressed);
	if (UndoButton)
		UndoButton->OnClicked.AddDynamic(this, &UEncounterLevelMakerWindow::OnUndoPressed);
	if (RedoButton)
		RedoButton->OnClicked.AddDynamic(this, &UEncounterLevelMakerWindow::OnRedoPressed);
	if (InputGridSizeX)
		InputGridSizeX->OnTextChanged.AddDynamic(this, &UEncounterLevelMakerWindow::OnGridTextXChange);
	if (InputGridSizeY)
		InputGridSizeY->OnTextChanged.AddDynamic(this, &UEncounterLevelMakerWindow::OnGridTextYChange);
	if (EnemyComboBox)
		EnemyComboBox->OnSelectionChanged.AddDynamic(this, &UEncounterLevelMakerWindow::OnEnemySelectChange);
	if (TrapComboBox)
		TrapComboBox->OnSelectionChanged.AddDynamic(this, &UEncounterLevelMakerWindow::OnTrapSelectChange);
	if (InteractableComboBox)
		InteractableComboBox->OnSelectionChanged.AddDynamic(this, &UEncounterLevelMakerWindow::OnInteractableSelectChange);
	if (PropsComboBox)
		PropsComboBox->OnSelectionChanged.AddDynamic(this, &UEncounterLevelMakerWindow::OnPropSelectChange);
	#pragma endregion 
	// adding choices to the editor
	#pragma region Data Table Info Fetch
	if (EnemyComboBox && EnemyDataTable)
	{
		TArray<FName> rowNames = EnemyDataTable->GetRowNames();
		for (FName& name : rowNames)
		{
			if (FEnemyInfoStruct* enemyInfo = EnemyDataTable->FindRow<FEnemyInfoStruct>(name, TEXT("Encounter Tool")))
				EnemyComboBox->AddOption(enemyInfo->EnemyName.ToString());
		}
	}
	if (TrapComboBox && TrapDataTable)
	{
		TArray<FName> rowNames = TrapDataTable->GetRowNames();
		for (FName& name : rowNames)
		{
			if (FTrapInfoStruct* trapInfo = TrapDataTable->FindRow<FTrapInfoStruct>(name, TEXT("Encounter Tool")))
				TrapComboBox->AddOption(trapInfo->TrapName.ToString());
		}
	}
	if (InteractableComboBox && InteractableDataTable)
	{
		TArray<FName> rowNames = InteractableDataTable->GetRowNames();
		for (FName& name : rowNames)
		{
			if (FInteractableInfoStruct* interactableInfo = InteractableDataTable->FindRow<FInteractableInfoStruct>(name, TEXT("Encounter Tool")))
				InteractableComboBox->AddOption(interactableInfo->InteractableName.ToString());
		}
	}
	if (PropsComboBox && WallToggleButton)
	{
		TArray<FName> rowNames = PropsDataTable->GetRowNames();
		for (FName& name : rowNames)
		{
			if (FAssetLookUpTable* interactableInfo = PropsDataTable->FindRow<FAssetLookUpTable>(name, TEXT("Encounter Tool")))
				PropsComboBox->AddOption(interactableInfo->AssetName.ToString());
		}
	}
	#pragma endregion
	// get and refresh the grid
	GridSize = FIntPoint(10,10);
	VisualiserObject = GetGridVisualiser();
	ResetAndPopulateGridArray();
	if (!LevelGridVisualISM)
		LevelGridVisualISM = VisualiserObject->GridVisualComp;
	// setting up undo keys
	MyInputProcessor = MakeShared<FUndoKeyInput>(this);
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().RegisterInputPreProcessor(MyInputProcessor);
	}
}
// So the code stops when menu is closed
void UEncounterLevelMakerWindow::NativeDestruct()
{
	Super::NativeDestruct();
	TimerManager->ClearTimer(HoverTimerHandle);
	// unbinding undo keys
	if (FSlateApplication::IsInitialized() && MyInputProcessor.IsValid())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(MyInputProcessor);
		MyInputProcessor.Reset();
	}
	if (VisualiserObject)
		VisualiserObject->Destroy();
}
#pragma region Button Functions
void UEncounterLevelMakerWindow::OnCreateGridPressed()
{
	// Converting to int and checking if it is valid
	FString xString =  InputGridSizeX->GetText().ToString();
	FString yString =  InputGridSizeY->GetText().ToString();
	int32 x = FCString::Atoi(*xString);
	int32 y = FCString::Atoi(*yString);
	if (xString == "" || yString == "" || xString == "0" || yString == "0")
	{
		x = 10;
		y = 10;
	}
	// double check if the visual grid is still there
	if (!VisualiserObject)
		VisualiserObject = GetGridVisualiser();
	// for undo
	FLevelEditorCellInfo newInfo = FLevelEditorCellInfo();
	newInfo.IsGridSizeChanged = true;
	newInfo.OldGridData = ConvertToExportData();
	// apply change
	GridSize = FIntPoint(x,y);
	ResetAndPopulateGridArray();
	VisualiserObject->RegenerateGrid(GridSize);
	// get the new state for undo and add undo session to list
	newInfo.NewGridData = ConvertToExportData();
	// no need to save undo if it nothing changed
	if (newInfo.NewGridData == newInfo.OldGridData)
		return;
	CurrentUndoSession.UndoInfo.Add(newInfo);
	AddUndoSession();
}
// Basically just check if button is pressed and set brush to according type
void UEncounterLevelMakerWindow::OnWalkableTogglePressed()
{
	UnselectCurrentButton();
	if (CurrentModeSelected == ECellBrushMode::Walkable)
	{
		CurrentModeSelected = ECellBrushMode::None;
	}
	else
	{
		CurrentModeSelected = ECellBrushMode::Walkable;
		WalkToggleButton->SetBackgroundColor(FLinearColor::Green);
		SetUpTrack();
	}
}
void UEncounterLevelMakerWindow::OnWallTogglePressed()
{
	UnselectCurrentButton();
	if (CurrentModeSelected == ECellBrushMode::Wall)
	{
		CurrentModeSelected = ECellBrushMode::None;
	}
	else
	{
		if (PropsComboBox->GetSelectedIndex() == 0)
		{
			CurrentModeSelected = ECellBrushMode::None;
			HighlightComboBox(PropsComboBox);
		}
		else
		{
			CurrentModeSelected = ECellBrushMode::Wall;
			WallToggleButton->SetBackgroundColor(FLinearColor::Green);
			SetUpTrack();
		}
	}
}
void UEncounterLevelMakerWindow::OnPlayerTogglePressed()
{
	UnselectCurrentButton();
	if (CurrentModeSelected == ECellBrushMode::PlayerSpawn || PlayerPlaced)
	{
		CurrentModeSelected = ECellBrushMode::None;
	}
	else
	{
		CurrentModeSelected = ECellBrushMode::PlayerSpawn;
		PlayerToggleButton->SetBackgroundColor(FLinearColor::Green);
		SetUpTrack();
	}
}
// these also check if user has selected an option from the list
void UEncounterLevelMakerWindow::OnEnemyTogglePressed()
{
	UnselectCurrentButton();
	if (CurrentModeSelected == ECellBrushMode::EnemySpawn)
	{
		CurrentModeSelected = ECellBrushMode::None;
	}
	else
	{
		if (EnemyComboBox->GetSelectedIndex() == 0)
		{
			CurrentModeSelected = ECellBrushMode::None;
			HighlightComboBox(EnemyComboBox);
		}
		else
		{
			CurrentModeSelected = ECellBrushMode::EnemySpawn;
			EnemyToggleButton->SetBackgroundColor(FLinearColor::Green);
			SetUpTrack();
		}
	}
}
void UEncounterLevelMakerWindow::OnTrapTogglePressed()
{
	UnselectCurrentButton();
	if (CurrentModeSelected == ECellBrushMode::Trap)
	{
		CurrentModeSelected = ECellBrushMode::None;
	}
	else
	{
		if (TrapComboBox->GetSelectedIndex() == 0)
		{
			CurrentModeSelected = ECellBrushMode::None;
			HighlightComboBox(TrapComboBox);
		}
		else
		{
			CurrentModeSelected = ECellBrushMode::Trap;
			TrapToggleButton->SetBackgroundColor(FLinearColor::Green);
			SetUpTrack();
		}
	}
}
void UEncounterLevelMakerWindow::OnInteractableTogglePressed()
{
	UnselectCurrentButton();
	if (CurrentModeSelected == ECellBrushMode::InteractableSpawn)
	{
		CurrentModeSelected = ECellBrushMode::None;
	}
	else
	{
		if (InteractableComboBox->GetSelectedIndex() == 0)
		{
			CurrentModeSelected = ECellBrushMode::None;
			HighlightComboBox(InteractableComboBox);
		}
		else
		{
			CurrentModeSelected = ECellBrushMode::InteractableSpawn;
			InteractableToggleButton->SetBackgroundColor(FLinearColor::Green);
			SetUpTrack();
		}
	}
}
/// Loads up selected file
void UEncounterLevelMakerWindow::OnLoadFilePressed()
{
	if (IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get())
	{
		// Basic setup json file search
		TArray<FString> OutFileNames;
		bool bOpened = DesktopPlatform->OpenFileDialog(
			nullptr, 
			TEXT("Select a File"), 
			TEXT("C:/"), 
			TEXT(""), 
			TEXT("JSON Files (*.json)|*.json|All Files (*.*)|*.*"), 
			EFileDialogFlags::None, 
			OutFileNames
		);
		// if selected a file
		if (bOpened && OutFileNames.Num() > 0)
		{
			FString SelectedFilePath = OutFileNames[0];
			FString jsonData;
			// fetch file data and shove it into "jsonData"
			if (FFileHelper::LoadFileToString(jsonData, *SelectedFilePath))
			{
				// Convert JSON to struct
				FLevelExportStruct exportData;
				if (FJsonObjectConverter::JsonObjectStringToUStruct(jsonData, &exportData, 0, 0))
				{
					// for undo
					FLevelEditorCellInfo newInfo = FLevelEditorCellInfo();
					newInfo.IsGridSizeChanged = true;
					newInfo.OldGridData = ConvertToExportData();
					// this will then process it
					ProcessExportData(exportData);
					// get the new state for undo and add undo session to list
					newInfo.NewGridData = exportData;
					CurrentUndoSession.UndoInfo.Add(newInfo);
					AddUndoSession();
				}
			}
		}
	}
}
/// Turns data into JSON file
void UEncounterLevelMakerWindow::OnExportPressed()
{
	if (!PlayerPlaced)
		return;
	if (IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get())
	{
		// set up to find desired save location
		FString SavePath;
		TArray<FString> OutFiles;
		bool bSaved = DesktopPlatform->SaveFileDialog(
			nullptr,
			TEXT("Export Level Data"),
			TEXT("C:/"),
			TEXT("MyLevel.json"),
			TEXT("JSON Files (*.json)|*.json"),
			EFileDialogFlags::None,
			OutFiles
		);
		// if chosen location
		if (bSaved && OutFiles.Num() > 0)
		{
			// turn into struct for JSON conversion 
			FLevelExportStruct exportData = ConvertToExportData();
			FString jsonData;
			// turns struct into JSON
			FJsonObjectConverter::UStructToJsonObjectString(exportData, jsonData);
			// Add it to user's files
			if (FFileHelper::SaveStringToFile(jsonData, *OutFiles[0]))
			{
				UE_LOG(LogTemp, Log, TEXT("Export Successful: %s"), *OutFiles[0]);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to save file to: %s"), *OutFiles[0]);
			}
		}
	}
}
// undo changes done to grid
void UEncounterLevelMakerWindow::OnUndoPressed()
{
	ActualUndo();
}
// works same as undo but revert to future changes
void UEncounterLevelMakerWindow::OnRedoPressed()
{
	ActualRedo();
}
#pragma endregion
#pragma region On Changes Functions
// validating input and limiting the grid size
void UEncounterLevelMakerWindow::OnGridTextXChange(const FText& NewText)
{
	FString stringText = NewText.ToString();
	if (!stringText.IsNumeric())
	{
		InputGridSizeX->SetText(FText::GetEmpty());
	}
	else
	{
		if (FCString::Atoi(*stringText) > 100)
			InputGridSizeX->SetText(FText::AsNumber(100));
	}
}
void UEncounterLevelMakerWindow::OnGridTextYChange(const FText& NewText)
{
	FString stringText = NewText.ToString();
	if (!stringText.IsNumeric())
	{
		InputGridSizeY->SetText(FText::GetEmpty());
	}
	else
	{
		if (FCString::Atoi(*stringText) > 100)
			InputGridSizeY->SetText(FText::AsNumber(100));
	}
}
// Determine if the list UI should remove its red highlight
void UEncounterLevelMakerWindow::OnEnemySelectChange(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectedItem == "None" && (ESelectInfo::OnMouseClick == SelectionType || ESelectInfo::OnKeyPress == SelectionType))
	{
		if (CurrentModeSelected != ECellBrushMode::EnemySpawn)
			return;
		UnselectCurrentButton();
		CurrentModeSelected = ECellBrushMode::None;
		HighlightComboBox(EnemyComboBox);
	}
	else
	{
		UnhighlightComboBox(EnemyComboBox);
		if (CurrentModeSelected == ECellBrushMode::EnemySpawn)
		{
			SelectSize = GetGroupSize(CurrentModeSelected);
		}
	}
}
void UEncounterLevelMakerWindow::OnTrapSelectChange(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectedItem == "None" && (ESelectInfo::OnMouseClick == SelectionType || ESelectInfo::OnKeyPress == SelectionType))
	{
		if (CurrentModeSelected != ECellBrushMode::Trap)
			return;
		UnselectCurrentButton();
		CurrentModeSelected = ECellBrushMode::None;
		HighlightComboBox(TrapComboBox);
	}
	else
	{
		UnhighlightComboBox(TrapComboBox);
		if (CurrentModeSelected == ECellBrushMode::Trap)
		{
			SelectSize = GetGroupSize(CurrentModeSelected);
		}
	}
}
void UEncounterLevelMakerWindow::OnInteractableSelectChange(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectedItem == "None" && (ESelectInfo::OnMouseClick == SelectionType || ESelectInfo::OnKeyPress == SelectionType))
	{
		if (CurrentModeSelected != ECellBrushMode::InteractableSpawn)
			return;
		UnselectCurrentButton();
		CurrentModeSelected = ECellBrushMode::None;
		HighlightComboBox(InteractableComboBox);
	}
	else
	{
		UnhighlightComboBox(InteractableComboBox);
		if (CurrentModeSelected == ECellBrushMode::InteractableSpawn)
		{
			SelectSize = GetGroupSize(CurrentModeSelected);
		}
	}
}

void UEncounterLevelMakerWindow::OnPropSelectChange(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectedItem == "None" && (ESelectInfo::OnMouseClick == SelectionType || ESelectInfo::OnKeyPress == SelectionType))
	{
		if (CurrentModeSelected != ECellBrushMode::Wall)
			return;
		UnselectCurrentButton();
		CurrentModeSelected = ECellBrushMode::None;
		HighlightComboBox(PropsComboBox);
	}
	else
	{
		UnhighlightComboBox(PropsComboBox);
		if (CurrentModeSelected == ECellBrushMode::Wall)
		{
			SelectSize = GetGroupSize(CurrentModeSelected);
		}
	}
}
#pragma endregion
#pragma region Utilities
// Update button UI to be unselected
void UEncounterLevelMakerWindow::UnselectCurrentButton()
{
	UnhighlightComboBox(EnemyComboBox);
	UnhighlightComboBox(TrapComboBox);
	UnhighlightComboBox(InteractableComboBox);
	if (TimerManager)
		TimerManager->ClearTimer(HoverTimerHandle);
	switch (CurrentModeSelected)
	{
	case ECellBrushMode::Walkable:
			WalkToggleButton->SetBackgroundColor(FLinearColor::White);
		break;
		case ECellBrushMode::Wall:
			WallToggleButton->SetBackgroundColor(FLinearColor::White);
		break;
		case ECellBrushMode::EnemySpawn:
			EnemyToggleButton->SetBackgroundColor(FLinearColor::White);
		break;
		case ECellBrushMode::Trap:
			TrapToggleButton->SetBackgroundColor(FLinearColor::White);
		break;
		case ECellBrushMode::InteractableSpawn:
			InteractableToggleButton->SetBackgroundColor(FLinearColor::White);
		break;
		case ECellBrushMode::PlayerSpawn:
			PlayerToggleButton->SetBackgroundColor(FLinearColor::White);
		break;
		default: 
		break;
	}
}
// Highlight and Unhighlight choice options with red
void UEncounterLevelMakerWindow::HighlightComboBox(UEditorUtilityComboBoxString* comboBox)
{
	FComboBoxStyle newStyle = comboBox->GetWidgetStyle();
	newStyle.ComboButtonStyle.ButtonStyle.Normal.TintColor = FSlateColor(FLinearColor::Red);
	newStyle.ComboButtonStyle.ButtonStyle.Hovered.TintColor = FSlateColor(FLinearColor::Red);
	newStyle.ComboButtonStyle.ButtonStyle.Pressed.TintColor = FSlateColor(FLinearColor::Red);
	comboBox->SetWidgetStyle(newStyle);
}
void UEncounterLevelMakerWindow::UnhighlightComboBox(UEditorUtilityComboBoxString* comboBox)
{
	FComboBoxStyle newStyle = comboBox->GetWidgetStyle();
	newStyle.ComboButtonStyle.ButtonStyle.Normal.TintColor = FSlateColor (FLinearColor::Black);
	newStyle.ComboButtonStyle.ButtonStyle.Hovered.TintColor = FSlateColor (FLinearColor::Black);
	newStyle.ComboButtonStyle.ButtonStyle.Pressed.TintColor = FSlateColor (FLinearColor::Black);
	comboBox->SetWidgetStyle(newStyle);
}
// Checks before running hover checks
void UEncounterLevelMakerWindow::SetUpTrack()
{
	// checking required stuff
	if (!World)
	{
		World = GEditor->GetEditorWorldContext().World();
		TimerManager = &World->GetTimerManager();
	}
	if (!VisualiserObject)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		VisualiserObject = GEditor->GetEditorWorldContext().World()->SpawnActor<AGridVisualiser>(VisualiserClass, FVector(50.0f, 50.0f, 0.0f), FRotator::ZeroRotator, SpawnParams);
		GridSize = FIntPoint(10,10);
			
		ResetAndPopulateGridArray();
	}
	if (!LevelGridVisualISM)
		LevelGridVisualISM = VisualiserObject->GridVisualComp;
	// reset values for new hover
	LastHoverIndex = -1;
	LastClickIndex = -1;
	// Set select size base on object we are placing down
	// Lets start with enemy for now (could make a function where it returns a grid)
	SelectSize = GetGroupSize(CurrentModeSelected);
	// start mouse detects / hover
	TimerManager->SetTimer(HoverTimerHandle, this, &UEncounterLevelMakerWindow::EnableMouseTrack, 0.01f, true);
}
// during mouse hover
void UEncounterLevelMakerWindow::EnableMouseTrack()
{
	// [NOTE] code probably move to set up to make more efficient
	// getting user screen and mouse pos
	FLevelEditorModule& LEModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
	TSharedPtr<SLevelViewport> activeViewport = LEModule.GetFirstActiveLevelViewport();
	if (!activeViewport.IsValid())
		return;
	FLevelEditorViewportClient& clientViewpoint = activeViewport->GetLevelViewportClient();
	FViewportCursorLocation cursorLoc = clientViewpoint.GetCursorWorldLocationFromMousePos();
	// setting up for raycast for ISM
	FVector rayStartPos = cursorLoc.GetOrigin();
	FVector rayDir = cursorLoc.GetDirection();
	FVector rayEndPos = rayStartPos + (rayDir * 10000.0f);
	FHitResult hitResult;
	FCollisionQueryParams params;
	params.bTraceComplex = true;
	// re-checks before 
	if (!World || !LevelGridVisualISM)
	{
		if (!World)
		{
			World = GEditor->GetEditorWorldContext().World();
			TimerManager = &World->GetTimerManager();
		}
		if (!VisualiserObject)
			VisualiserObject = GetGridVisualiser();
		if (!LevelGridVisualISM)
			LevelGridVisualISM = VisualiserObject->GridVisualComp;
		
		TimerManager->ClearTimer(HoverTimerHandle);
		UnselectCurrentButton();
		CurrentModeSelected = ECellBrushMode::None;
		return;
	}
	// raycast
	if (World->LineTraceSingleByChannel(hitResult, rayStartPos, rayEndPos, ECC_GameTraceChannel1, params))
	{
		// if hit an ISM
		if (UInstancedStaticMeshComponent* ISMC = Cast<UInstancedStaticMeshComponent>(hitResult.GetComponent()))
		{
			// if not hovering over the right grid
			if (ISMC != LevelGridVisualISM)
			{
				UnhoverGridCell();
				LastHoverIndex = -1;
				return;
			}
			// get ISM's child index
			int32 hitIndex = hitResult.Item;
			FIntPoint gridPos;
			gridPos.Y = hitIndex / GridSize.X;
			gridPos.X = hitIndex % GridSize.X;
			// remove the hover effect from previous hit cells
			if (LastHoverIndex != hitIndex)
			{
				if (LastHoverIndex != -1)
				{
					for (auto pos : SelectedCells)
					{
						if (FLevelCellData* cellData = LevelGridData.Find(pos.GridPos))
						{
							ISMC->SetCustomData(pos.IndexPos, GetColour(cellData->CellType), false);
						}
					}
				}
				LastHoverIndex = hitIndex;
				LastHoverPos = gridPos;
				SelectedGroupMainCellIndex = hitIndex;
				CalculateSelectCells(gridPos);
			}
			if (Obstructed)
				return;
			// if player is hold or clicking
			if (clientViewpoint.Viewport->KeyState(EKeys::LeftMouseButton))
			{
				if (LastClickIndex == hitIndex)
					return;
				// for undo
				if (!CursorDown)
				{
					CursorDown = true;
				}
				LastClickIndex = hitIndex;
				if (FLevelCellData* CellData = LevelGridData.Find(gridPos))
				{
					// for UNDO action
					FLevelEditorCellInfo newInfo = FLevelEditorCellInfo();
					newInfo.GridPos = gridPos;
					newInfo.CellData = *CellData;
					newInfo.IsmIndex = hitIndex;
					// updating data grid
					CellData->CellType = CurrentModeSelected;
					// storing ID if enemy, trap or Interactable
					CellData->IdForCellContent = GetRelevantObjID(CurrentModeSelected);
					// redo data for undo action
					newInfo.RedoCellData = *CellData;
					CurrentUndoSession.UndoInfo.Add(newInfo);
					// apply text to show ID if applicable
					SetUpTextComp(CellData, &newInfo.CellData, hitIndex);
					HandleNeighbouringCells(hitIndex);
				}
				// set colour
				ISMC->SetCustomData(hitIndex, GetColour(CurrentModeSelected), false);
				// for player spawn button lock
				if (PlayerSpawnPos == gridPos && PlayerPlaced)
				{
					PlayerPlaced = false;
					PlayerSpawnPos = FIntPoint(-1, -1);
					PlayerToggleButton->SetIsEnabled(true);
					return;
				}
				if (CurrentModeSelected == ECellBrushMode::PlayerSpawn)
				{
					PlayerSpawnPos = gridPos;
					PlayerPlaced = true;
					TimerManager->ClearTimer(HoverTimerHandle);
					UnselectCurrentButton();
					CurrentModeSelected = ECellBrushMode::None;
					PlayerToggleButton->SetIsEnabled(false);
					CursorDown = false;
					AddUndoSession();
				}
			}
			else if (CursorDown)
			{
				CursorDown = false;
				AddUndoSession();
			}
			return;
		}
	}
	// unselect if not hovering over grid
	if (LastHoverIndex != -1)
	{
		UnhoverGridCell();
		LastHoverIndex = -1;
	}
}
// Refresh the grid basically
void UEncounterLevelMakerWindow::ResetAndPopulateGridArray()
{
	PlayerPlaced = false;
	PlayerSpawnPos = FIntPoint(-1, -1);
	PlayerToggleButton->SetIsEnabled(true);
	LevelGridData.Empty();
	for (int x = 0; x < GridSize.X; x++)
	{
		for (int y = 0; y < GridSize.Y; y++)
		{
			LevelGridData.FindOrAdd(FIntPoint(x, y));
		}
	}
}
// Extract info from the export and apply it
void UEncounterLevelMakerWindow::ProcessExportData(FLevelExportStruct exportData)
{
	GridSize = exportData.GridSize;
	ResetAndPopulateGridArray();
	VisualiserObject->RegenerateGrid(GridSize);
	// for each cell with content
	for (auto& cellData : exportData.GridCells)
	{
		if (FLevelCellData* cellInfo = LevelGridData.Find(cellData.Position))
		{
			if (PlayerPlaced && cellData.CellType == ECellBrushMode::PlayerSpawn)
			{
				UE_LOG(LogTemp, Log, TEXT("Why is there another player spawn point? Oh well, cell (%d, %d) IGNORED!"), cellData.Position.X, cellData.Position.Y);
				continue;
			}
			if (cellData.CellType == ECellBrushMode::Walkable)
			{
				LastClickIndex = -1;
			}
			int32 ismIndex = (cellData.Position.Y * GridSize.X) + cellData.Position.X;
			FLevelCellData oldType = *cellInfo;
			*cellInfo = FLevelCellData(cellData.CellType, cellData.IdForCellContent);
			// to highlight the visual grid
			LevelGridVisualISM->SetCustomData(ismIndex, GetColour( cellData.CellType), true);
			if (SetUpTextComp(cellInfo, &oldType, ismIndex))
				continue;
			// disable the player spawn brush if there is alr a spawn on grid
			if (cellData.CellType == ECellBrushMode::PlayerSpawn)
			{
				PlayerPlaced = true;
				PlayerSpawnPos = cellData.Position;
				PlayerToggleButton->SetIsEnabled(false);
			}
		}
	}
}
// removes the hover effect
void UEncounterLevelMakerWindow::UnhoverGridCell()
{
	for (int x = 0; x < SelectSize.X; x++)
	{
		for (int y = 0; y < SelectSize.Y; y++)
		{
			FIntPoint checkPos = LastHoverPos + FIntPoint(x, y);
			int32 checkIsmIndex = (checkPos.Y * GridSize.X) + checkPos.X;
			if (FLevelCellData* CellData = LevelGridData.Find(checkPos))
			{
				LevelGridVisualISM->SetCustomData(checkIsmIndex, GetColour(CellData->CellType), false);
			}
		}
	}
}
// gets reference to selected cells
void UEncounterLevelMakerWindow::CalculateSelectCells(FIntPoint gridPos)
{
	SelectedCells.Empty();
	Obstructed = false;
	// basically check each cell in the select shape
	for (int x = 0; x < SelectSize.X; x++)
	{
		for (int y = 0; y < SelectSize.Y; y++)
		{
			FIntPoint checkPos = gridPos + FIntPoint(x, y);
			int32 checkIsmIndex = (checkPos.Y * GridSize.X) + checkPos.X;
			if (FLevelCellData* CellData = LevelGridData.Find(checkPos))
			{
				SelectedCells.Add(FGridPosAnd1DIndex(checkPos, checkIsmIndex));
				if (CurrentModeSelected == ECellBrushMode::Walkable)
				{
					SelectedCells.Append(GetGroupedCells(CellData));
					continue;
				}
				if (Obstructed)
					continue;
				Obstructed = !(CellData->CellType == ECellBrushMode::None || CellData->CellType == ECellBrushMode::Walkable);
			}
		}
	}
	// colour hit cell yellow
	auto hoverColor =  { 1.0f, 0.8f, 0.0f, 1.0f };
	if (SelectedCells.Num() < SelectSize.X * SelectSize.Y || Obstructed)
	{
		Obstructed = true;
		hoverColor = { 1.0f, 0.5f, 0.0f, 1.0f }	;
	}
	for (int i = 0; i < SelectedCells.Num(); i++)
	{
		LevelGridVisualISM->SetCustomData(SelectedCells[i].IndexPos, hoverColor, false);
	}
}
// if one of the group member is deleted, delete its mates
void UEncounterLevelMakerWindow::HandleNeighbouringCells(int32 placedIndex)
{
	for (auto pos : SelectedCells)
	{
		if (pos.IndexPos == placedIndex)
			continue;
		if (FLevelCellData* neighbourCellData = LevelGridData.Find(pos.GridPos))
		{
			FLevelEditorCellInfo newInfo = FLevelEditorCellInfo();
			newInfo.GridPos = pos.GridPos;
			newInfo.CellData = *neighbourCellData;
			newInfo.IsmIndex = pos.IndexPos;
			if (CurrentModeSelected == ECellBrushMode::EnemySpawn)
				newInfo.RedoCellData = FLevelCellData(ECellBrushMode::EnemyChunk, placedIndex);
			else if (CurrentModeSelected == ECellBrushMode::Walkable)
				newInfo.RedoCellData = FLevelCellData(CurrentModeSelected);
			else
				newInfo.RedoCellData = FLevelCellData(CurrentModeSelected, placedIndex);
			CurrentUndoSession.UndoInfo.Add(newInfo);
			*neighbourCellData = newInfo.RedoCellData;
			LevelGridVisualISM->SetCustomData(pos.IndexPos, GetColour(CurrentModeSelected), false);
			SetUpTextComp(neighbourCellData, &newInfo.CellData, pos.IndexPos);
		}
	}
}
// fetch the right info from the right data table
int32 UEncounterLevelMakerWindow::GetRelevantObjID(ECellBrushMode type) const
{
	switch (type)
	{
		case ECellBrushMode::EnemySpawn:
			return EnemyComboBox->GetSelectedIndex();
		case ECellBrushMode::Trap:
			return TrapComboBox->GetSelectedIndex();
		case ECellBrushMode::InteractableSpawn:
			return  InteractableComboBox->GetSelectedIndex();
		case ECellBrushMode::Wall:
			return PropsComboBox->GetSelectedIndex();
		default:
			return -1;
	}
}
// gets all cells relating to that group of cells
TArray<FGridPosAnd1DIndex> UEncounterLevelMakerWindow::GetGroupedCells(FLevelCellData* selectedCellData)
{
	TArray<FGridPosAnd1DIndex> groupedCells;
	if (selectedCellData->CellType == ECellBrushMode::EnemyChunk || selectedCellData->CellType == ECellBrushMode::EnemySpawn)
	{
		if (selectedCellData->CellType == ECellBrushMode::EnemyChunk)
			SelectedGroupMainCellIndex = selectedCellData->IdForCellContent;
		// get where the main group body was actually placed 
		FIntPoint mainGridPos;
		mainGridPos.Y = SelectedGroupMainCellIndex / GridSize.X;
		mainGridPos.X = SelectedGroupMainCellIndex % GridSize.X;
		if (FLevelCellData* mainData = LevelGridData.Find(mainGridPos))
		{
			FString stringConvert = FString::FromInt(mainData->IdForCellContent);
			if (FEnemyInfoStruct* enemyInfo = EnemyDataTable->FindRow<FEnemyInfoStruct>(FName(*stringConvert), TEXT("Encounter Tool")))
			{
				groupedCells.Add(FGridPosAnd1DIndex(mainGridPos, SelectedGroupMainCellIndex));
				// use this to find all other members
				FIntPoint groupSize =  enemyInfo->GridSize;
				for (int x2 = 0; x2 < groupSize.X; x2++)
				{
					for (int y2 = 0; y2 < groupSize.Y; y2++)
					{
						if (FIntPoint(x2, y2) == FIntPoint(0))
							continue;
						FIntPoint checkGroupPos = mainGridPos + FIntPoint(x2, y2);
						int32 checkGroupIsmIndex = (checkGroupPos.Y * GridSize.X) + checkGroupPos.X;
						if (checkGroupIsmIndex == SelectedGroupMainCellIndex)
							continue;
						if (LevelGridData.Find(checkGroupPos))
						{
							groupedCells.Add(FGridPosAnd1DIndex(checkGroupPos, checkGroupIsmIndex));
						}
					}
				}
			}
		}
	}
	return groupedCells;
}
// Prepare an export struct for export
FLevelExportStruct UEncounterLevelMakerWindow::ConvertToExportData()
{
	FLevelExportStruct newExport;
	newExport.GridSize = GridSize;
	for (const auto& [gridPos, data] : LevelGridData)
	{
		// only records the special cells
		if (data.CellType == ECellBrushMode::None || data.CellType == ECellBrushMode::Walkable)
			continue;
		newExport.GridCells.Add(FExportCellDetails(gridPos, data.CellType, data.IdForCellContent));
	}
	return newExport;
}
// Get highlight colour for that cell
TArray<float> UEncounterLevelMakerWindow::GetColour(ECellBrushMode type)
{
	switch (type)
	{
		case ECellBrushMode::Wall:
			return { 0.0f, 0.0f, 0.0f, 1.0f};
		case ECellBrushMode::EnemySpawn:
			return{ 1.0f, 0.0f, 0.0f, 1.0f};
		case ECellBrushMode::EnemyChunk:
			return { 1.0f, 0.0f, 0.0f, 1.0f};
		case ECellBrushMode::Trap:
			return { 0.0f, 0.0f, 1.0f, 1.0f};
		case ECellBrushMode::InteractableSpawn:
			return { 0.0f, 0.5f, 1.0f, 1.0f};
		case ECellBrushMode::PlayerSpawn:
			return  { 0.0f, 1.0f, 0.0f, 1.0f};
		default:
			return { 1.0f, 1.0f, 1.0f, 1.0f};
	}
}
// Needed to see the grid visually
TObjectPtr<AGridVisualiser> UEncounterLevelMakerWindow::GetGridVisualiser()
{
	TObjectPtr<AGridVisualiser> foundVisualiser = Cast<AGridVisualiser>(UGameplayStatics::GetActorOfClass(GEditor->GetEditorWorldContext().World(), AGridVisualiser::StaticClass()));
	// create one if not found in world
	if (!foundVisualiser)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		foundVisualiser = GEditor->GetEditorWorldContext().World()->SpawnActor<AGridVisualiser>(VisualiserClass, FVector(50.0f, 50.0f, 0.0f), FRotator::ZeroRotator, SpawnParams);
	}
	foundVisualiser->RegenerateGrid(GridSize);
	return foundVisualiser;
}
// checks if Text component needs to change, be added or be removed
bool UEncounterLevelMakerWindow::SetUpTextComp(const FLevelCellData* newData, const FLevelCellData* prevData, int32 ismIndex)
{
	int32 prevID = prevData->IdForCellContent;
	int32 newID = newData->IdForCellContent;
	// don't add text if it is an extension to another cell
	switch (prevData->CellType)
	{
		case ECellBrushMode::EnemyChunk:
			prevID = -1;
		break;
		default:
		break;
	}
	switch (newData->CellType)
	{
		case ECellBrushMode::EnemyChunk:
			newID = -1;
		break;
		default:
		break;
	}
	bool prevHasText = prevID != -1;
	bool needText = newID != -1;
	// if there's already a text
	if (needText && prevHasText)
	{
		VisualiserObject->SetText(ismIndex, FText::AsNumber(newID), GetGroupSize(newData->CellType, newID, false));
	}
	// if there is no text
	else if (needText)
	{
		VisualiserObject->AddText(ismIndex, FText::AsNumber(newID), GetGroupSize(newData->CellType, newID, false));
	}
	// if there's no need for text any more
	else if (prevHasText)
	{
		VisualiserObject->DeleteText(ismIndex);
	}
	else
	{
		return false;
	}
	return true;
}
FIntPoint UEncounterLevelMakerWindow::GetGroupSize(ECellBrushMode type, int32 id, bool isSelectMode)
{
	if (id == -1 && isSelectMode)
		id = GetRelevantObjID(type);
	FString stringConvert = FString::FromInt(id);
	switch (type)
	{
	case ECellBrushMode::EnemySpawn:
			if (FEnemyInfoStruct* enemyInfo = EnemyDataTable->FindRow<FEnemyInfoStruct>(FName(*stringConvert), TEXT("Encounter Tool")))
				return enemyInfo->GridSize;
		break;
		default:
		break;
	}
	return FIntPoint(1);
}
// add session to undo list to later undo or redo
void UEncounterLevelMakerWindow::AddUndoSession()
{
	UndoIndex++;
	UndoList.Add(CurrentUndoSession);
	CurrentUndoSession.UndoInfo.Empty();
	// clears the redo when a new action is done
	if (UndoIndex + 1 < UndoList.Num())
	{
		int32 ElementsToRemove = UndoList.Num() - UndoIndex;
		UndoList.RemoveAt(UndoIndex + 1, ElementsToRemove);
	}
}

bool UEncounterLevelMakerWindow::ActualUndo()
{
	if (!UndoList.IsValidIndex(UndoIndex - 1))
		return false;
	UndoIndex--;
	// loop through all actions done in that single undo session 
	// (e.g. if you hold mouse down and colour many grid before letting go)
	for (const auto& UndoAction : UndoList[UndoIndex].UndoInfo)
	{
		// if it is related to changing grid size
		if (UndoAction.IsGridSizeChanged)
		{
			ProcessExportData(UndoAction.OldGridData);
			break;
		}
		// get pos and set that grid cell to what it was previously 
		if (FLevelCellData* CellData = LevelGridData.Find(UndoAction.GridPos))
		{
			*CellData = UndoAction.CellData;
		}
		LevelGridVisualISM->SetCustomData(UndoAction.IsmIndex, GetColour(UndoAction.CellData.CellType), true);
		// if it has object ID like trap, Interactable (e.g., etc.)
		if (SetUpTextComp(&UndoAction.CellData, &UndoAction.RedoCellData, UndoAction.IsmIndex))
			continue;
		// player button toggle things
		if (UndoAction.RedoCellData.CellType == ECellBrushMode::PlayerSpawn)
		{
			PlayerPlaced = false;
			PlayerSpawnPos = FIntPoint(-1, -1);
			PlayerToggleButton->SetIsEnabled(true);
		}
		else if (UndoAction.CellData.CellType == ECellBrushMode::PlayerSpawn)
		{
			PlayerPlaced = true;
			PlayerSpawnPos = UndoAction.GridPos;
			PlayerToggleButton->SetIsEnabled(false);
		}
	}
	return true;
}
bool UEncounterLevelMakerWindow::ActualRedo()
{
	if (!UndoList.IsValidIndex(UndoIndex))
		return false;
	UE_LOG(LogTemp, Warning, TEXT("Undo Index: %d"), UndoIndex);
	for (const auto& UndoAction : UndoList[UndoIndex].UndoInfo)
	{
		if (UndoAction.IsGridSizeChanged)
		{
			ProcessExportData(UndoAction.NewGridData);
			break;
		}
		if (FLevelCellData* CellData = LevelGridData.Find(UndoAction.GridPos))
		{
			*CellData = UndoAction.RedoCellData;
		}
		LevelGridVisualISM->SetCustomData(UndoAction.IsmIndex, GetColour( UndoAction.RedoCellData.CellType), true);
		if (SetUpTextComp(&UndoAction.RedoCellData, &UndoAction.CellData, UndoAction.IsmIndex))
			continue;
		if ( UndoAction.RedoCellData.CellType == ECellBrushMode::PlayerSpawn)
		{
			PlayerPlaced = true;
			PlayerSpawnPos = UndoAction.GridPos;
			PlayerToggleButton->SetIsEnabled(false);
		}
		else if (UndoAction.CellData.CellType == ECellBrushMode::PlayerSpawn)
		{
			PlayerPlaced = false;
			PlayerSpawnPos = FIntPoint(-1);
			PlayerToggleButton->SetIsEnabled(true);
		}
	}
	UndoIndex++;
	return true;
}
#pragma endregion

