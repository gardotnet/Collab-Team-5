#include "GridRangeMakerWindow.h"

#if WITH_EDITOR
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
	
	if (!World)
	{
		World = GEditor->GetEditorWorldContext().World();
		TimerManager = &World->GetTimerManager();
	}
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
	FString xString =  InputGridSizeX->GetText().ToString();
	FString yString =  InputGridSizeY->GetText().ToString();
	int32 x = FCString::Atoi(*xString);
	int32 y = FCString::Atoi(*yString);
	if (xString == "" || yString == "" || xString == "0" || yString == "0")
	{
		x = 3;
		y = 3;
	}
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
		VisualiserObject->Modify();
	}
	if (!LevelGridVisualISM)
		LevelGridVisualISM = VisualiserObject->GridVisualComp;
	
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

void UGridRangeMakerWindow::OnNorthButtonClicked()
{
	SelectDirectionsButton(NorthButton, ECompassDirection::N);
}
void UGridRangeMakerWindow::OnNorthEastButtonClicked()
{
	SelectDirectionsButton(NorthEastButton, ECompassDirection::NE);
}
void UGridRangeMakerWindow::OnEastButtonClicked()
{
	SelectDirectionsButton(EastButton, ECompassDirection::E);
}
void UGridRangeMakerWindow::OnSouthEastButtonClicked()
{
	SelectDirectionsButton(SouthEastButton, ECompassDirection::SE);
}
void UGridRangeMakerWindow::OnSouthButtonClicked()
{
	SelectDirectionsButton(SouthButton, ECompassDirection::S);
}
void UGridRangeMakerWindow::OnSouthWestButtonClicked()
{
	SelectDirectionsButton(SouthWestButton, ECompassDirection::SW);
}
void UGridRangeMakerWindow::OnWestButtonClicked()
{
	SelectDirectionsButton(WestButton, ECompassDirection::W);
}
void UGridRangeMakerWindow::OnNorthWestButtonClicked()
{
	SelectDirectionsButton(NorthWestButton, ECompassDirection::NW);
}

void UGridRangeMakerWindow::OnLoadLevelFileButtonClicked()
{
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
					GridSize = exportData.GridSize;
					ResetAndPopulateGridArray();
					VisualiserObject->RegenerateGrid(GridSize);
					for (auto info : exportData.Cells)
					{
						FIntPoint origPos = info.GridPos + exportData.CentrePoint;
						if (FEditorGridRangeCellState* cellInfo = LevelGridData.Find(origPos))
						{
							cellInfo->CellType = EGridRangeBrushMode::Slot;
							cellInfo->GroupID = info.GroupID;
							for (auto modifiers : info.DirectionModifier)
							{
								if (modifiers.Direction == ECompassDirection::N)
									cellInfo->N = modifiers.Magnitude;
								if (modifiers.Direction == ECompassDirection::NE)
									cellInfo->NE = modifiers.Magnitude;
								if (modifiers.Direction == ECompassDirection::E)
									cellInfo->E = modifiers.Magnitude;
								if (modifiers.Direction == ECompassDirection::SE)
									cellInfo->SE = modifiers.Magnitude;
								if (modifiers.Direction == ECompassDirection::S)
									cellInfo->S = modifiers.Magnitude;
								if (modifiers.Direction == ECompassDirection::SW)
									cellInfo->SW = modifiers.Magnitude;
								if (modifiers.Direction == ECompassDirection::W)
									cellInfo->W = modifiers.Magnitude;
								if (modifiers.Direction == ECompassDirection::NW)
									cellInfo->NW = modifiers.Magnitude;
							}
							int32 ismIndex = (origPos.Y * GridSize.X) + origPos.X;
							LevelGridVisualISM->SetCustomData(ismIndex, GetColour(cellInfo->CellType), true);
						}
					}
					if (FEditorGridRangeCellState* centreInfo = LevelGridData.Find(exportData.CentrePoint))
					{
						centreInfo->CellType = EGridRangeBrushMode::Centre;
						CentrePlaced = true;
						CentrePoint = exportData.CentrePoint;
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
	if (CentrePoint == FIntPoint(-1, -1))
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
				highestX = FMath::Max(highestX, gridPos.X);
				highestY = FMath::Max(highestY, gridPos.Y);
				if (OneGroup->GetCheckedState() == ECheckBoxState::Checked)
					newInfo.GroupID = 0;
				else
					newInfo.GroupID = data.GroupID;
				if (data.N > -1)
					newInfo.DirectionModifier.Add(FDirectionAndMag(ECompassDirection::N, data.N));
				if (data.NE > -1)
					newInfo.DirectionModifier.Add(FDirectionAndMag(ECompassDirection::NE, data.NE));
				if (data.E > -1)
					newInfo.DirectionModifier.Add(FDirectionAndMag(ECompassDirection::E, data.E));
				if (data.SE > -1)
					newInfo.DirectionModifier.Add(FDirectionAndMag(ECompassDirection::SE, data.SE));
				if (data.S > -1)
					newInfo.DirectionModifier.Add(FDirectionAndMag(ECompassDirection::S, data.S));
				if (data.SW > -1)
					newInfo.DirectionModifier.Add(FDirectionAndMag(ECompassDirection::SW, data.SW));
				if (data.W > -1)
					newInfo.DirectionModifier.Add(FDirectionAndMag(ECompassDirection::W, data.W));
				if (data.NW > -1)
					newInfo.DirectionModifier.Add(FDirectionAndMag(ECompassDirection::NW, data.NW));
				newExportStruct.Cells.Add(newInfo);
			}
			newExportStruct.GridSize = FIntPoint(highestX + 1, highestY + 1);
			if (OneGroup->GetCheckedState() == ECheckBoxState::Checked)
				newExportStruct.OneGroup = true;
			FString jsonData;
			// turns struct into JSON
			FJsonObjectConverter::UStructToJsonObjectString(newExportStruct, jsonData);
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
		InputDirMag->SetText(FText::FromString("INF"));
		if (MagnitudeChanging)
			*MagnitudeChanging = 0;
	}
	else if (stringText == "-" || stringText == "-1")
	{
		InputDirMag->SetText(FText::FromString("N/A"));
		if (MagnitudeChanging)
			*MagnitudeChanging = -1;
	}
	if (stringText == "")
	{
		InputDirMag->SetText(FText::FromString(stringText));
		if (MagnitudeChanging)
			*MagnitudeChanging = -1;
	}
	else if (!stringText.IsNumeric())
	{
		InputDirMag->SetText(FText::FromString(PrevMagText));
		if (MagnitudeChanging)
			*MagnitudeChanging = -1;
		return;
	}
	else
	{
		if (MagnitudeChanging)
			*MagnitudeChanging = FCString::Atoi(*stringText);
	}
	//*infoPtr = info;
	PrevMagText = stringText;
}

void UGridRangeMakerWindow::OnGroupChange(const FText& NewText)
{
	FString stringText = NewText.ToString();
	if (stringText == "-" || stringText == "-1")
	{
		InputGroupId->SetText(FText::FromString("N/A"));
		GroupLabel->SetIsEnabled(false);
		infoPtr->GroupID = -1;
	}
	else if (stringText == "")
	{
		InputGroupId->SetText(FText::FromString(stringText));
		GroupLabel->SetIsEnabled(false);
		infoPtr->GroupID = -1;
	}
	else if (!stringText.IsNumeric())
	{
		InputGroupId->SetText(FText::FromString(PrevGroupText));
		return;
	}
	else
	{
		infoPtr->GroupID = FCString::Atoi(*stringText);
		GroupLabel->SetIsEnabled(true);
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
			
		if (!LevelGridVisualISM)
		{
			if (VisualiserObject)
				LevelGridVisualISM = VisualiserObject->GridVisualComp;
		}
		return;
	}
	FLevelEditorModule& LEModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
	TSharedPtr<SLevelViewport> ActiveViewport = LEModule.GetFirstActiveLevelViewport();
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
				ISMC->SetCustomData(hitIndex, { 1.0f, 0.8f, 0.0f, 1.0f }, false);
			}
			if (CentrePoint == gridPos && CurrentModeSelected != EGridRangeBrushMode::Erase)
				return;
			
			if (clientViewpoint.Viewport->KeyState(EKeys::LeftMouseButton))
			{ 
				if (LastClickIndex == hitIndex && CurrentModeSelected != EGridRangeBrushMode::None && CurrentModeSelected != EGridRangeBrushMode::Erase)
					return;
				if (MouseDownForSelect)
					return;
				/*FScopedTransaction Transaction(FText::FromString("Grid Edit"));
				this->Modify();
				ISMC->Modify();*/
				LastClickIndex = hitIndex;
				if (FEditorGridRangeCellState* CellData = LevelGridData.Find(LastHoverPos))
				{
					switch (CurrentModeSelected)
					{
					case EGridRangeBrushMode::Erase:
						if (CellData->CellType == EGridRangeBrushMode::Centre)
						{
							CentrePlaced = false;
							CentrePoint = FIntPoint(-1, -1);
							CentreButton->SetIsEnabled(true);
						}
						else if (CellData->CellType == EGridRangeBrushMode::Select)
						{
							DirectionCanvas->SetIsEnabled(false);
							CellNameLabel->SetText(FText::FromString("Selected Cell: N/A"));
							LastSelectedIndex = -1;
							infoPtr = nullptr;
						}
						CellData->CellType = EGridRangeBrushMode::None;
						CellData->ClearModifier();
						break;
					case EGridRangeBrushMode::Slot:
						if (CellData->CellType == EGridRangeBrushMode::Slot)
						{
							LastClickIndex = -1;
							return;
						}
						UE_LOG(LogTemp, Warning, TEXT("Slot"))
						if (LastSelectedIndex != -1)
						{
							infoPtr->CellType = EGridRangeBrushMode::Slot;
							ISMC->SetCustomData(LastSelectedIndex, GetColour(infoPtr->CellType), true);
						}
						CellData->CellType = EGridRangeBrushMode::Select;
						LastSelectedIndex = hitIndex;
						CurrentDirSelected = ECompassDirection::None;
						infoPtr = CellData;
						LoadDirectionsOnCell(gridPos);
						
						break;
					case EGridRangeBrushMode::Centre:
						UE_LOG(LogTemp, Warning, TEXT("Centre"))
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
						UE_LOG(LogTemp, Warning, TEXT("Select Check"))
						if (CellData->CellType != EGridRangeBrushMode::Slot && CellData->CellType != EGridRangeBrushMode::Select)
						{
							LastClickIndex = -1;
							return;
						}
						if (CellData->CellType == EGridRangeBrushMode::Select)
						{
							UE_LOG(LogTemp, Warning, TEXT("Deselecting"))
							DirectionCanvas->SetIsEnabled(false);
							CellNameLabel->SetText(FText::FromString("Selected Cell: N/A"));
							CellData->CellType = EGridRangeBrushMode::Slot;
							ISMC->SetCustomData(LastSelectedIndex, GetColour(CellData->CellType), false);
							LastSelectedIndex = -1;
							infoPtr = nullptr;
							MouseDownForSelect = true;
							return;
						}
						//info = *CellData;
						
						if (LastSelectedIndex != -1)
						{
							infoPtr->CellType = EGridRangeBrushMode::Slot;
							ISMC->SetCustomData(LastSelectedIndex, GetColour(CellData->CellType), false);
						}
						CellData->CellType = EGridRangeBrushMode::Select;
						infoPtr = CellData;
						LastSelectedIndex = hitIndex;
						UE_LOG(LogTemp, Warning, TEXT("Selecting"))
						CurrentDirSelected = ECompassDirection::None;
						LoadDirectionsOnCell(gridPos);
						MouseDownForSelect = true;
						break;
					default:
						break;
					}
					ISMC->SetCustomData(hitIndex, GetColour(CellData->CellType), false);
				}
			}
			else
			{
				MouseDownForSelect = false;
			}
		}
		else
		{
			if (LastHoverIndex != -1)
			{
				UnselectCell();
			}
		}
	}
	else
	{
		if (LastHoverIndex != -1)
		{
			UnselectCell();
		}
	}
}

void UGridRangeMakerWindow::ResetAndPopulateGridArray()
{
	CentrePlaced = false;
	CentrePoint = FIntPoint(-1, -1);
	LastHoverPos = FIntPoint(-1, -1);
	LastHoverIndex = -1;
	LastClickIndex = -1;
	LastSelectedIndex = -1;
	LevelGridData.Empty();
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

void UGridRangeMakerWindow::UnselectDirectionButton()
{
	switch (CurrentDirSelected)
	{
		case ECompassDirection::N:
			//if (info.N > -1)
			if (infoPtr->N > -1)
				NorthButton->SetBackgroundColor(FLinearColor::Green);
			else
				NorthButton->SetBackgroundColor(FLinearColor::White);
			break;
		case ECompassDirection::NE:
			//if (info.NE > -1)
			if (infoPtr->NE > -1)
				NorthEastButton->SetBackgroundColor(FLinearColor::Green);
			else
				NorthEastButton->SetBackgroundColor(FLinearColor::White);
			break;
		case ECompassDirection::E:
			//if (info.E > -1)
			if (infoPtr->E > -1)
				EastButton->SetBackgroundColor(FLinearColor::Green);
			else
				EastButton->SetBackgroundColor(FLinearColor::White);
			break;
		case ECompassDirection::SE:
			//if (info.SE > -1)
			if (infoPtr->SE > -1)
				SouthEastButton->SetBackgroundColor(FLinearColor::Green);
			else
				SouthEastButton->SetBackgroundColor(FLinearColor::White);
			break;
		case ECompassDirection::S:
			//if (info.S > -1)
			if (infoPtr->S > -1)
				SouthButton->SetBackgroundColor(FLinearColor::Green);
			else
				SouthButton->SetBackgroundColor(FLinearColor::White);
			break;
		case ECompassDirection::SW:
			//if (info.SW > -1)
			if (infoPtr->SW > -1)
				SouthWestButton->SetBackgroundColor(FLinearColor::Green);
			else
				SouthWestButton->SetBackgroundColor(FLinearColor::White);
			break;
		case ECompassDirection::W:
			//if (info.W > -1)
			if (infoPtr->W > -1)
				WestButton->SetBackgroundColor(FLinearColor::Green);
			else
				WestButton->SetBackgroundColor(FLinearColor::White);
			break;
		case ECompassDirection::NW:
			//if (info.NW > -1)
			if (infoPtr->NW > -1)
				NorthWestButton->SetBackgroundColor(FLinearColor::Green);
			else
				NorthWestButton->SetBackgroundColor(FLinearColor::White);
			break;
		default:
		break;
	}
}

void UGridRangeMakerWindow::HideInputsForCell()
{
	InputDirMag->SetText(FText::GetEmpty());
	InputDirMag->SetIsEnabled(false);
	DirLabel->SetIsEnabled(false);
	DirectionCanvas->SetIsEnabled(true);
}

void UGridRangeMakerWindow::LoadDirectionsOnCell(FIntPoint pos)
{
	FString posString = "[ " + FString::FromInt(pos.X) + ", " + FString::FromInt(pos.Y) + " ]";
	CellNameLabel->SetText(FText::FromString("Selected Cell: " + posString));
	HideInputsForCell();
	RefreshInputForGroup();
	
	NorthButton->SetBackgroundColor(FLinearColor::White);
	NorthEastButton->SetBackgroundColor(FLinearColor::White);
	EastButton->SetBackgroundColor(FLinearColor::White);
	SouthEastButton->SetBackgroundColor(FLinearColor::White);
	SouthButton->SetBackgroundColor(FLinearColor::White);
	SouthWestButton->SetBackgroundColor(FLinearColor::White);
	WestButton->SetBackgroundColor(FLinearColor::White);
	NorthWestButton->SetBackgroundColor(FLinearColor::White);
	//if (info.N > -1)
	if (infoPtr->N > -1)
		NorthButton->SetBackgroundColor(FLinearColor::Green);
	//if (info.NE > -1)
	if (infoPtr->NE > -1)
		NorthEastButton->SetBackgroundColor(FLinearColor::Green);
	//if (info.E > -1)
	if (infoPtr->E > -1)
		EastButton->SetBackgroundColor(FLinearColor::Green);
	//if (info.SE > -1)
	if (infoPtr->SE > -1)
		SouthEastButton->SetBackgroundColor(FLinearColor::Green);
	//if (info.S > -1)
	if (infoPtr->S > -1)
		SouthButton->SetBackgroundColor(FLinearColor::Green);
	//if (info.SW > -1)
	if (infoPtr->SW > -1)
		SouthWestButton->SetBackgroundColor(FLinearColor::Green);
	//if (info.W > -1)
	if (infoPtr->W > -1)
		WestButton->SetBackgroundColor(FLinearColor::Green);
	//if (info.NW > -1)
	if (infoPtr->NW > -1)
		NorthWestButton->SetBackgroundColor(FLinearColor::Green);
}
void UGridRangeMakerWindow::SelectDirectionsButton(UEditorUtilityButton* button, ECompassDirection dir)
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
		button->SetBackgroundColor(FLinearColor::Yellow);
		MagnitudeChanging = GetMagnitudeOfDirection();
		if (*MagnitudeChanging == -1)
		{
			InputDirMag->SetText(FText::GetEmpty());
		}
		else if (*MagnitudeChanging == 0)
		{
			InputDirMag->SetText(FText::FromString("INF"));
		}
		else
		{
			InputDirMag->SetText(FText::AsNumber(*MagnitudeChanging));
		}
	}
}

void UGridRangeMakerWindow::RefreshInputForGroup()
{
	if (infoPtr->GroupID > -1)
	{
		InputGroupId->SetText(FText::AsNumber(infoPtr->GroupID));
		GroupLabel->SetIsEnabled(true);
	}
	else
	{
		InputGroupId->SetText(FText::GetEmpty());
		GroupLabel->SetIsEnabled(false);
	}
}

int32* UGridRangeMakerWindow::GetMagnitudeOfDirection()
{
	switch (CurrentDirSelected)
	{
	case ECompassDirection::N:
		//return &info.N;
		return &infoPtr->N;
	case ECompassDirection::NE:
		//return &info.NE;
		return &infoPtr->NE;
	case ECompassDirection::E:
		//return &info.E;
		return &infoPtr->E;
	case ECompassDirection::SE:
		//return &info.SE;
		return &infoPtr->SE;
	case ECompassDirection::S:
		//return &info.S;
		return &infoPtr->S;
	case ECompassDirection::SW:
		//return &info.SW;
		return &infoPtr->SW;
	case ECompassDirection::W:
		//return &info.W;
		return &infoPtr->W;
	case ECompassDirection::NW:
		//return &info.NW;
		return &infoPtr->NW;
	default:
		return nullptr;
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

#endif
