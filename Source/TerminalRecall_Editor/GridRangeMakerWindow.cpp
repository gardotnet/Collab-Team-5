#include "GridRangeMakerWindow.h"

#include "DesktopPlatformModule.h"
#include "EditorUtilityWidgetComponents.h"
#include "IDesktopPlatform.h"
#include "JsonObjectConverter.h"
#include "Kismet/GameplayStatics.h"
#include "LevelEditor.h"
#include "SLevelViewport.h"
#include "Components/CanvasPanel.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/TextBlock.h"
// A modified and IMPROVED VERSION(maybe) of "Battle Level Editor"
void UGridRangeMakerWindow::NativeConstruct()
{
	Super::NativeConstruct();
	HoverTimerHandle = FTimerHandle();
	#pragma region UI Links
	if (CreateGridButton)
		CreateGridButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::OnCreateGrid);
	if (EraseButton)
		EraseButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::OnEraseButtonClicked);
	if (SlotDrawButton)
		SlotDrawButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::OnPaintButtonClicked);
	if (CentreButton)
		CentreButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::SetCentrePoint);
	if (NorthButton)
		NorthButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::OnNorthButtonClicked);
	if (NorthEastButton)
		NorthEastButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::OnNorthEastButtonClicked);
	if (EastButton)
		EastButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::OnEastButtonClicked);
	if (SouthEastButton)
		SouthEastButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::OnSouthEastButtonClicked);
	if (SouthButton)
		SouthButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::OnSouthButtonClicked);
	if (SouthWestButton)
		SouthWestButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::OnSouthWestButtonClicked);
	if (WestButton)
		WestButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::OnWestButtonClicked);
	if (NorthWestButton)
		NorthWestButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::OnNorthWestButtonClicked);
	if (LoadLevelFileButton)
		LoadLevelFileButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::OnLoadLevelFileButtonClicked);
	if (ExportButton)
		ExportButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::OnExportButtonClicked);
	/*if (UndoButton)
		UndoButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::OnUndoButtonClicked);
	if (RedoButton)
		RedoButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::OnRedoButtonClicked);*/
	if (InputGridSizeX)
		InputGridSizeX->OnTextChanged.AddDynamic(this, &UGridRangeMakerWindow::OnGridTextXChange);
	if (InputGridSizeY)
		InputGridSizeY->OnTextChanged.AddDynamic(this, &UGridRangeMakerWindow::OnGridTextYChange);
	if (InputDirMag)
		InputDirMag->OnTextChanged.AddDynamic(this, &UGridRangeMakerWindow::OnDirMagChange);
	if (InputGroupId)
		InputGroupId->OnTextChanged.AddDynamic(this, &UGridRangeMakerWindow::OnGroupChange);
	if (OneGroup)
		OneGroup->OnCheckStateChanged.AddDynamic(this, &UGridRangeMakerWindow::OnCheckOneGroup);
	#pragma endregion 
	// to reference easily throughout this script
	#pragma region Button Mapping
	DirButtons.Add(ECompassDirection::N ,NorthButton);
	DirButtons.Add(ECompassDirection::NE ,NorthEastButton);
	DirButtons.Add(ECompassDirection::E ,EastButton);
	DirButtons.Add(ECompassDirection::SE ,SouthEastButton);
	DirButtons.Add(ECompassDirection::S ,SouthButton);
	DirButtons.Add(ECompassDirection::SW ,SouthWestButton);
	DirButtons.Add(ECompassDirection::W ,WestButton);
	DirButtons.Add(ECompassDirection::NW ,NorthWestButton);
	#pragma endregion
	World = GEditor->GetEditorWorldContext().World();
	TimerManager = &World->GetTimerManager();
	// start mouse track
	TimerManager->SetTimer(HoverTimerHandle, this, &UGridRangeMakerWindow::TrackMouse, 0.01f, true);
}
void UGridRangeMakerWindow::NativeDestruct()
{
	Super::NativeDestruct();
	TimerManager->ClearTimer(HoverTimerHandle);
	if (VisualiserObject)
		VisualiserObject->Destroy();
}
#pragma region Bind Functions
void UGridRangeMakerWindow::OnCreateGrid()
{
	FScopedTransaction Transaction(FText::FromString("Grid Create"));
	this->Modify();
	// Converting to int and checking if it is valid
	int32 x = FCString::Atoi(*InputGridSizeX->GetText().ToString());
	int32 y = FCString::Atoi(*InputGridSizeY->GetText().ToString());
	if (x <= 0) x = 3;
	if (y <= 0) y = 3;
	// Create grid or just reset it
	if (!World)
	{
		World = GEditor->GetEditorWorldContext().World();
		TimerManager = &World->GetTimerManager();
	}
	if (!VisualiserObject)
	{
		VisualiserObject = Cast<AGridVisualiser>(UGameplayStatics::GetActorOfClass(World, AGridVisualiser::StaticClass()));
		if (!VisualiserObject)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			VisualiserObject = GEditor->GetEditorWorldContext().World()->SpawnActor<AGridVisualiser>(VisualiserClass, FVector(50.0f, -7950.0f, 0.0f), FRotator::ZeroRotator, SpawnParams);
		}
	}
	if (!LevelGridVisualISM)
		LevelGridVisualISM = VisualiserObject->GridVisualComp;
	DirectionCanvas->SetIsEnabled(false);
	CellNameLabel->SetText(FText::FromString("Selected Cell: N/A"));
	WasClicked = false;
	SelectedCellIndexes.Empty();
	GridSize = FIntPoint(x,y);
	ResetAndPopulateGridArray();
	VisualiserObject->RegenerateGrid(GridSize);
}
void UGridRangeMakerWindow::OnEraseButtonClicked()
{
	UnselectCurrentButton();
	if (CurrentModeSelected == EGridRangeBrushMode::Erase)
	{
		CurrentModeSelected = EGridRangeBrushMode::None;
	}
	else
	{
		CurrentModeSelected = EGridRangeBrushMode::Erase;
		EraseButton->SetBackgroundColor(FLinearColor::Green);
	}
}
void UGridRangeMakerWindow::OnPaintButtonClicked()
{
	UnselectCurrentButton();
	if (CurrentModeSelected == EGridRangeBrushMode::Slot)
	{
		CurrentModeSelected = EGridRangeBrushMode::None;
	}
	else
	{
		CurrentModeSelected = EGridRangeBrushMode::Slot;
		SlotDrawButton->SetBackgroundColor(FLinearColor::Green);
	}
}
void UGridRangeMakerWindow::SetCentrePoint()
{
	UnselectCurrentButton();
	if (CurrentModeSelected == EGridRangeBrushMode::Centre)
	{
		CurrentModeSelected = EGridRangeBrushMode::None;
	}
	else
	{
		CurrentModeSelected = EGridRangeBrushMode::Centre;
		CentreButton->SetBackgroundColor(FLinearColor::Green);
	}
}
void UGridRangeMakerWindow::OnNorthButtonClicked() { SelectDirectionsButton(ECompassDirection::N);}
void UGridRangeMakerWindow::OnNorthEastButtonClicked() { SelectDirectionsButton(ECompassDirection::NE);}
void UGridRangeMakerWindow::OnEastButtonClicked() { SelectDirectionsButton(ECompassDirection::E);}
void UGridRangeMakerWindow::OnSouthEastButtonClicked() { SelectDirectionsButton(ECompassDirection::SE);}
void UGridRangeMakerWindow::OnSouthButtonClicked(){SelectDirectionsButton(ECompassDirection::S);}
void UGridRangeMakerWindow::OnSouthWestButtonClicked(){SelectDirectionsButton(ECompassDirection::SW);}
void UGridRangeMakerWindow::OnWestButtonClicked(){SelectDirectionsButton(ECompassDirection::W);}
void UGridRangeMakerWindow::OnNorthWestButtonClicked(){SelectDirectionsButton(ECompassDirection::NW);}
void UGridRangeMakerWindow::OnLoadLevelFileButtonClicked()
{
	// making sure there is something to load into
	if (!VisualiserObject)
	{
		VisualiserObject = Cast<AGridVisualiser>(UGameplayStatics::GetActorOfClass(World, AGridVisualiser::StaticClass()));
		if (!VisualiserObject)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			VisualiserObject = GEditor->GetEditorWorldContext().World()->SpawnActor<AGridVisualiser>(VisualiserClass, FVector(50.0f, -7950.0f, 0.0f), FRotator::ZeroRotator, SpawnParams);
		}
	}
	if (!LevelGridVisualISM)
	{
		if (VisualiserObject)
			LevelGridVisualISM = VisualiserObject->GridVisualComp;
	}
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
				FGridRangeExport exportData;
				if (FJsonObjectConverter::JsonObjectStringToUStruct(jsonData, &exportData, 0, 0))
				{
					GridSize = GridSize.ComponentMax(exportData.GridSize);
					FIntPoint newCentre =  exportData.CentrePoint + ((GridSize - exportData.GridSize)/2);
					ResetAndPopulateGridArray();
					VisualiserObject->RegenerateGrid(GridSize);
					for (auto info : exportData.Cells)
					{
						FIntPoint origPos = info.GridPos + newCentre;
						if (FEditorGridRangeCellState* cellInfo = LevelGridData.Find(origPos))
						{
							cellInfo->CellType = EGridRangeBrushMode::Slot;
							cellInfo->GroupID = info.GroupID;
							for (auto modifiers : info.DirectionModifier)
							{
								cellInfo->DirectionMags[modifiers.Direction] = modifiers.Magnitude;
							}
							int32 ismIndex = (origPos.Y * GridSize.X) + origPos.X;
							LevelGridVisualISM->SetCustomData(ismIndex, GetColour(cellInfo->CellType), true);
						}
					}
					if (FEditorGridRangeCellState* centreInfo = LevelGridData.Find(newCentre))
					{
						centreInfo->CellType = EGridRangeBrushMode::Centre;
						CentrePlaced = true;
						CentrePoint = newCentre;
						int32 ismIndex = (CentrePoint.Y * GridSize.X) + CentrePoint.X;
						LevelGridVisualISM->SetCustomData(ismIndex, GetColour(centreInfo->CellType), true);
					}
					if (exportData.OneGroup)
					{
						OneGroup->SetCheckedState(ECheckBoxState::Checked);
						InputGroupId->SetIsEnabled(false);
					}
					else
					{
						OneGroup->SetCheckedState(ECheckBoxState::Unchecked);
						InputGroupId->SetIsEnabled(true);
					}
				}
			}
		}
	}
}
void UGridRangeMakerWindow::OnExportButtonClicked()
{
	if (!CentrePlaced)
		return;
	if (IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get())
	{
		// set up to find desired save location
		FString SavePath;
		TArray<FString> OutFiles;
		bool bSaved = DesktopPlatform->SaveFileDialog(
			nullptr,
			TEXT("Export Grid Range Data"),
			TEXT("C:/"),
			TEXT("MyGridRange.json"),
			TEXT("JSON Files (*.json)|*.json"),
			EFileDialogFlags::None,
			OutFiles
		);
		// if chosen location
		if (bSaved && OutFiles.Num() > 0)
		{
			// turn into struct for JSON conversion 
			FGridRangeExport newExportStruct;
			newExportStruct.CentrePoint = CentrePoint;
			int32 highestX = TNumericLimits<int32>::Min();
			int32 highestY = TNumericLimits<int32>::Min();
			for (const auto& [gridPos, data] : LevelGridData)
			{
				if ( data.CellType == EGridRangeBrushMode::None || data.CellType == EGridRangeBrushMode::Centre)
					continue;
				FGridRangeCellInfo newInfo;
				newInfo.GridPos = gridPos - CentrePoint;
				// calculation to find what the minimum grid size should be at the end
				highestX = FMath::Max(highestX, gridPos.X);
				highestY = FMath::Max(highestY, gridPos.Y);
				// make every cell's group ID to be the same
				if (OneGroup->GetCheckedState() == ECheckBoxState::Checked)
					newInfo.GroupID = 0;
				else
					newInfo.GroupID = data.GroupID;
				// adding direction modifiers
				for (auto dirInfo : data.DirectionMags)
				{
					if (dirInfo.Value > -1)
						newInfo.DirectionModifier.Add(FDirectionAndMag(dirInfo.Key, dirInfo.Value));
				}
				newExportStruct.Cells.Add(newInfo);
			}
			newExportStruct.GridSize = FIntPoint(highestX + 1, highestY + 1);
			if (OneGroup->GetCheckedState() == ECheckBoxState::Checked)
				newExportStruct.OneGroup = true;
			FString jsonData;
			// turns struct into JSON
			FJsonObjectConverter::UStructToJsonObjectString(newExportStruct, jsonData);
			// Add it to user's files
			if (!FFileHelper::SaveStringToFile(jsonData, *OutFiles[0]))
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to save file to: %s"), *OutFiles[0]);
			}
		}
	}
}
void UGridRangeMakerWindow::OnGridTextXChange(const FText& NewText)
{
	FString stringText = NewText.ToString();
	if (stringText == "")
		InputGridSizeX->SetText(FText::FromString(stringText));
	else if (!stringText.IsNumeric())
	{
		InputGridSizeX->SetText(FText::FromString(PrevXSizeText));
		return;
	}
	else if (FCString::Atoi(*stringText) > 50)
		InputGridSizeX->SetText(FText::AsNumber(50));
	PrevXSizeText = stringText;
}
void UGridRangeMakerWindow::OnGridTextYChange(const FText& NewText)
{
	FString stringText = NewText.ToString();
	if (stringText == "")
		InputGridSizeY->SetText(FText::FromString(stringText));
	else if (!stringText.IsNumeric())
	{
		InputGridSizeY->SetText(FText::FromString(PrevYSizeText));
		return;
	}
	else if (FCString::Atoi(*stringText) > 50)
		InputGridSizeY->SetText(FText::AsNumber(50));
	PrevYSizeText = stringText;
}
void UGridRangeMakerWindow::OnDirMagChange(const FText& NewText)
{
	FString stringText = NewText.ToString();
	if (stringText == "0" || stringText == "INF")
	{
		stringText = "";
		InputDirMag->SetText(FText::FromString("INF"));
		SetDirectionMag(0);
	}
	else if (stringText == "-" || stringText == "-1")
	{
		// so we don't load a "-", when wrong input is given
		stringText = "";
		InputDirMag->SetText(FText::FromString("N/A"));
		SetDirectionMag(-1);
	}
	else if (stringText == "")
	{
		InputDirMag->SetText(FText::FromString(stringText));
		SetDirectionMag(-1);
	}
	else if (!stringText.IsNumeric())
	{
		InputDirMag->SetText(FText::FromString(PrevMagText));
		SetDirectionMag(-1);
		return;
	}
	else
	{
		SetDirectionMag(FCString::Atoi(*stringText));
	}
	PrevMagText = stringText;
}
void UGridRangeMakerWindow::OnGroupChange(const FText& NewText)
{
	FString stringText = NewText.ToString();
	int32 groupID = -1;
	if (stringText == "-" || stringText == "-1")
	{
		stringText = "";
		InputGroupId->SetText(FText::FromString("N/A"));
		GroupLabel->SetIsEnabled(false);
	}
	else if (stringText == "")
	{
		GroupLabel->SetIsEnabled(false);
	}
	else if (!stringText.IsNumeric())
	{
		InputGroupId->SetText(FText::FromString(PrevGroupText));
		return;
	}
	else
	{
		groupID = FCString::Atoi(*stringText);
		GroupLabel->SetIsEnabled(true);
	}
	for (auto pos : SelectedCellIndexes)
	{
		if (FEditorGridRangeCellState* CellData = LevelGridData.Find(pos))
		{
			CellData->GroupID = groupID;
		}
	}
	PrevGroupText = stringText;
}
void UGridRangeMakerWindow::OnCheckOneGroup(bool isChecked)
{
	if (!isChecked)
	{
		InputGroupId->SetIsEnabled(true);
		RefreshInputForGroup();
	}
	else
	{
		InputGroupId->SetIsEnabled(false);
		GroupLabel->SetIsEnabled(false);
	}
}
#pragma endregion
void UGridRangeMakerWindow::TrackMouse()
{
	// checks
	if (!World || !VisualiserObject || !LevelGridVisualISM)
	{
		if (!World)
		{
			World = GEditor->GetEditorWorldContext().World();
			TimerManager = &World->GetTimerManager();
		}
		if (LevelGridVisualISM)
			return;
		if (VisualiserObject)
			LevelGridVisualISM = VisualiserObject->GridVisualComp;
		return;
	}
	TSharedPtr<SLevelViewport> ActiveViewport = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor").GetFirstActiveLevelViewport();
	if (!ActiveViewport.IsValid())
		return;
	FLevelEditorViewportClient& clientViewpoint = ActiveViewport->GetLevelViewportClient();
	FViewportCursorLocation cursorLoc = clientViewpoint.GetCursorWorldLocationFromMousePos();
	// setting up for raycast for ISM
	FVector rayStartPos = cursorLoc.GetOrigin();
	FVector rayDir = cursorLoc.GetDirection();
	FVector rayEndPos = rayStartPos + (rayDir * 10000.0f);
	FHitResult hitResult;
	FCollisionQueryParams params;
	params.bTraceComplex = true;
	// raycast
	if (World->LineTraceSingleByChannel(hitResult, rayStartPos, rayEndPos, ECC_GameTraceChannel1, params))
	{
		// if hit an ISM
		if (UInstancedStaticMeshComponent* ISMC = Cast<UInstancedStaticMeshComponent>(hitResult.GetComponent()))
		{
			// if not hovering over the right grid
			if (ISMC != LevelGridVisualISM)
			{
				UnselectCell();
				return;
			}
			// get ISM's child index
			int32 hitIndex = hitResult.Item;
			FIntPoint gridPos;
			gridPos.Y = hitIndex / GridSize.X;
			gridPos.X = hitIndex % GridSize.X;
			if (LastHoverIndex != hitIndex)
			{
				if (LastHoverIndex != -1)
				{
					if (FEditorGridRangeCellState* CellData = LevelGridData.Find(LastHoverPos))
					{
						ISMC->SetCustomData(LastHoverIndex, GetColour(CellData->CellType), false);
					}
				}
				LastHoverIndex = hitIndex;
				LastHoverPos = gridPos;
				WasClicked = false;
				ISMC->SetCustomData(hitIndex, { 1.0f, 0.8f, 0.0f, 1.0f }, false);
			}
			if (CentrePoint == gridPos && CurrentModeSelected != EGridRangeBrushMode::Erase)
				return;
			if (clientViewpoint.Viewport->KeyState(EKeys::LeftMouseButton))
			{
				if (WasClicked && CurrentModeSelected != EGridRangeBrushMode::None)
					return;
				if (MouseDownForSelect && WasClicked)
					return;
				bool ctrlDown = clientViewpoint.Viewport->KeyState(EKeys::LeftControl) || clientViewpoint.Viewport->KeyState(EKeys::RightControl);
				WasClicked = true;
				if (FEditorGridRangeCellState* CellData = LevelGridData.Find(LastHoverPos))
				{
					switch (CurrentModeSelected)
					{
					case EGridRangeBrushMode::Erase:
						if (CellData->CellType == EGridRangeBrushMode::None)
							return;
						if (CellData->CellType == EGridRangeBrushMode::Centre)
						{
							CentrePlaced = false;
							CentrePoint = FIntPoint(-1, -1);
							CentreButton->SetIsEnabled(true);
						}
						else if (CellData->CellType == EGridRangeBrushMode::Select)
						{
							UnselectDirectionButton();
							SelectedCellIndexes.Remove(gridPos);
							if (SelectedCellIndexes.IsEmpty())
							{
								DirectionCanvas->SetIsEnabled(false);
								CellNameLabel->SetText(FText::FromString("Selected Cell: N/A"));
							}
						}
						CellData->CellType = EGridRangeBrushMode::None;
						CellData->ClearModifier();
						break;
					case EGridRangeBrushMode::Slot:
						if (CellData->CellType == EGridRangeBrushMode::Slot)
							return;
						RemoveSelectedCells();
						CellData->CellType = EGridRangeBrushMode::Select;
						AddSelectCell(gridPos);
						break;
					case EGridRangeBrushMode::Centre:
						CellData->CellType = EGridRangeBrushMode::Centre;
						CentrePlaced = true;
						CentrePoint = gridPos;
						CentreButton->SetIsEnabled(false);
						CellData->ClearModifier();
						ISMC->SetCustomData(hitIndex, GetColour(CellData->CellType), false);
						UnselectCurrentButton();
						CurrentModeSelected = EGridRangeBrushMode::None;
						break;
					case EGridRangeBrushMode::None:
						if (CellData->CellType != EGridRangeBrushMode::Slot && CellData->CellType != EGridRangeBrushMode::Select)
						{
							MouseDownForSelect = true;
							return;
						}
						if (CellData->CellType == EGridRangeBrushMode::Select)
						{
							CellData->CellType = EGridRangeBrushMode::Slot;
							ISMC->SetCustomData(hitIndex, GetColour(CellData->CellType), false);
							SelectedCellIndexes.Remove(gridPos);
							if (SelectedCellIndexes.IsEmpty())
							{
								DirectionCanvas->SetIsEnabled(false);
								CellNameLabel->SetText(FText::FromString("Selected Cell: N/A"));
								MouseDownForSelect = true;
								return;
							}
							if (ctrlDown)
							{
								MouseDownForSelect = true;
								return;
							}
						}
						if (!MouseDownForSelect && !ctrlDown)
						{
							RemoveSelectedCells();
						}
						CellData->CellType = EGridRangeBrushMode::Select;
						AddSelectCell(gridPos);
						MouseDownForSelect = true;
						break;
					default:
						break;
					}
					ISMC->SetCustomData(hitIndex, GetColour(CellData->CellType), false);
				}
				return;
			}
			MouseDownForSelect = false;
			return;
		}
		return;
	}
	if (LastHoverIndex != -1)
	{
		UnselectCell();
	}
}
void UGridRangeMakerWindow::ResetAndPopulateGridArray()
{
	CentrePlaced = false;
	CentrePoint = FIntPoint(-1, -1);
	CentreButton->SetIsEnabled(true);
	LastHoverPos = FIntPoint(-1, -1);
	LastHoverIndex = -1;
	WasClicked = false;
	LevelGridData.Empty();
	RemoveSelectedCells();
	for (int x = 0; x < GridSize.X; x++)
	{
		for (int y = 0; y < GridSize.Y; y++)
		{
			LevelGridData.FindOrAdd(FIntPoint(x, y));
		}
	}
}
void UGridRangeMakerWindow::UnselectCurrentButton()
{
	switch (CurrentModeSelected)
	{
	case EGridRangeBrushMode::Erase:
		EraseButton->SetBackgroundColor(FLinearColor::White);
		break;
	case EGridRangeBrushMode::Slot:
		SlotDrawButton->SetBackgroundColor(FLinearColor::White);
		break;
	case EGridRangeBrushMode::Centre:
		CentreButton->SetBackgroundColor(FLinearColor::White);
		break;
	default:
		break;
	}
}
void UGridRangeMakerWindow::UnselectCell()
{
	if (FEditorGridRangeCellState* CellData = LevelGridData.Find(LastHoverPos))
	{
		LevelGridVisualISM->SetCustomData(LastHoverIndex, GetColour(CellData->CellType), false);
	}
	LastHoverIndex = -1;
}
TArray<float> UGridRangeMakerWindow::GetColour(EGridRangeBrushMode cellType)
{
	switch (cellType)
	{
	case EGridRangeBrushMode::Slot:
		return {0.0f, 0.0f, 0.0f, 1.0f};
	case EGridRangeBrushMode::Centre:
		return {0.0f, 1.0f, 0.0f, 1.0f};
	case EGridRangeBrushMode::Select:
		return {1.0f, 0.5f, 1.0f, 1.0f};
	default:
		return {1.0f, 1.0f, 1.0f, 1.0f};
	}
}
void UGridRangeMakerWindow::UnselectDirectionButton()
{
	if (CurrentDirSelected == ECompassDirection::None)
		return;
	if (FEditorGridRangeCellState* firstSelectedData = LevelGridData.Find(SelectedCellIndexes[0]))
	{
		if (SelectedCellIndexes.Num() > 1)
		{
			bool sameValue = true;
			for (auto selectedCellsPos : SelectedCellIndexes)
			{
				if (FEditorGridRangeCellState* selectedData = LevelGridData.Find(selectedCellsPos))
				{
					if (selectedData->DirectionMags[CurrentDirSelected] != firstSelectedData->DirectionMags[CurrentDirSelected])
					{
						sameValue = false;
						break;
					}
				}
			}
			if (sameValue)
				DirButtons[CurrentDirSelected]->SetBackgroundColor(FLinearColor(1.0f, 0.5f, 1.0f, 1.0f));
			else
				DirButtons[CurrentDirSelected]->SetBackgroundColor(FLinearColor::Green);
		}
		if (firstSelectedData->DirectionMags[CurrentDirSelected] > -1)
			DirButtons[CurrentDirSelected]->SetBackgroundColor(FLinearColor::Green);
		else
		{
			DirButtons[CurrentDirSelected]->SetBackgroundColor(FLinearColor::White);
		}
		return;
	}
	for (auto button : DirButtons)
	{
		button.Value->SetBackgroundColor(FLinearColor::White);
	}
}
void UGridRangeMakerWindow::HideInputsForCell()
{
	InputDirMag->SetText(FText::GetEmpty());
	InputDirMag->SetIsEnabled(false);
	DirLabel->SetIsEnabled(false);
	DirectionCanvas->SetIsEnabled(true);
	UnselectDirectionButton();
}
void UGridRangeMakerWindow::LoadDirectionsOnCell(FIntPoint pos)
{
	FString posString = "[ " + FString::FromInt(pos.X) + ", " + FString::FromInt(pos.Y) + " ]";
	CellNameLabel->SetText(FText::FromString("Selected Cell: " + posString));
	HideInputsForCell();
	RefreshInputForGroup();
	if (FEditorGridRangeCellState* CellData = LevelGridData.Find(pos))
	{
		for (auto dirInfo : CellData->DirectionMags)
		{
			if (dirInfo.Value > -1)
				DirButtons[dirInfo.Key]->SetBackgroundColor(FLinearColor::Green);
			else
				DirButtons[dirInfo.Key]->SetBackgroundColor(FLinearColor::White);
		}
	}
}
void UGridRangeMakerWindow::SelectDirectionsButton(ECompassDirection dir)
{
	UnselectDirectionButton();
	if (CurrentDirSelected == dir)
	{
		HideInputsForCell();
		CurrentDirSelected = ECompassDirection::None;
	}
	else
	{
		InputDirMag->SetIsEnabled(true);
		DirLabel->SetIsEnabled(true);
		CurrentDirSelected = dir;
		DirButtons[dir]->SetBackgroundColor(FLinearColor::Yellow);
		if (FEditorGridRangeCellState* CellData = LevelGridData.Find(SelectedCellIndexes[0]))
		{
			if (SelectedCellIndexes.Num() > 1)
			{
				InputDirMag->SetText(FText::GetEmpty());
				return;
			}
			int32 magnitudeChanging = CellData->DirectionMags[CurrentDirSelected];
			if (magnitudeChanging == -1)
			{
				InputDirMag->SetText(FText::GetEmpty());
			}
			else if (magnitudeChanging == 0)
			{
				InputDirMag->SetText(FText::FromString("INF"));
			}
			else
			{
				InputDirMag->SetText(FText::AsNumber(magnitudeChanging));
			}
		}
	}
}
void UGridRangeMakerWindow::RefreshInputForGroup()
{
	if (FEditorGridRangeCellState* CellData = LevelGridData.Find(SelectedCellIndexes[0]))
	{
		if (SelectedCellIndexes.Num() > 1)
		{
			bool sameValue = true;
			for (auto selectedCellsPos : SelectedCellIndexes)
			{
				if (FEditorGridRangeCellState* selectedData = LevelGridData.Find(selectedCellsPos))
				{
					if (selectedData->GroupID != CellData->GroupID)
					{
						sameValue = false;
						break;
					}
				}
			}
			if (sameValue)
			{
				InputGroupId->SetText(FText::AsNumber(CellData->GroupID));
				GroupLabel->SetIsEnabled(true);
			}
			else
			{
				InputGroupId->SetText(FText::GetEmpty());
				GroupLabel->SetIsEnabled(false);
			}
			return;
		}
		if (CellData->GroupID > -1)
		{
			InputGroupId->SetText(FText::AsNumber(CellData->GroupID));
			GroupLabel->SetIsEnabled(true);
		}
		else
		{
			InputGroupId->SetText(FText::GetEmpty());
			GroupLabel->SetIsEnabled(false);
		}
	}
}
void UGridRangeMakerWindow::SetDirectionMag(int32 mag)
{
	if (SelectedCellIndexes.IsEmpty())
		return;
	for (auto pos : SelectedCellIndexes)
	{
		if (FEditorGridRangeCellState* CellData = LevelGridData.Find(pos))
		{
			CellData->DirectionMags[CurrentDirSelected] = mag;
		}
	}
}
void UGridRangeMakerWindow::AddSelectCell(FIntPoint pos)
{
	CurrentDirSelected = ECompassDirection::None;
	SelectedCellIndexes.AddUnique(pos);
	if (SelectedCellIndexes.Num() == 1)
	{
		LoadDirectionsOnCell(pos);
	}
	else
	{
		CellNameLabel->SetText(FText::FromString("Multiple Cells Selected"));
		for (auto buttons : DirButtons)
		{
			buttons.Value->SetBackgroundColor(FLinearColor(1.0f, 0.5f, 1.0f, 1.0f));
		}
	}
	
}
void UGridRangeMakerWindow::RemoveSelectedCells()
{
	if (SelectedCellIndexes.IsEmpty())
		return;
	for (auto pos : SelectedCellIndexes)
	{
		if (FEditorGridRangeCellState* CellData = LevelGridData.Find(pos))
		{
			CellData->CellType = EGridRangeBrushMode::Slot;
			int32 ismIndex = (pos.Y * GridSize.X) + pos.X;
			LevelGridVisualISM->SetCustomData(ismIndex, GetColour(EGridRangeBrushMode::Slot), false);
		}
	}
	SelectedCellIndexes.Empty();
}