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
	if (CloseDirMenuButton)
		CloseDirMenuButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::CloseDirMenu);
	if (SelectButton)
		SelectButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::OnSelectButtonClicked);
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
	
	#pragma endregion 
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
		SetUpMouseTrack();
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
		SetUpMouseTrack();
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
		SetUpMouseTrack();
	}
}

void UGridRangeMakerWindow::OnSelectButtonClicked()
{
	UnselectCurrentButton();
	if (CurrentModeSelected == EGridRangeBrushMode::Select)
	{
		CurrentModeSelected = EGridRangeBrushMode::None;
	}
	else
	{
		CurrentModeSelected = EGridRangeBrushMode::Select;
		SelectButton->SetBackgroundColor(FLinearColor::Green);
		SetUpMouseTrack();
	}
}

void UGridRangeMakerWindow::CloseDirMenu()
{
	UnselectCell();
	DirectionCanvas->SetVisibility(ESlateVisibility::Hidden);
	InputDirMag->SetIsEnabled(false);
	SetUpMouseTrack();
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
			for (const auto& [gridPos, data] : LevelGridData)
			{
				if ( data.CellType == EGridRangeBrushMode::None || data.CellType == EGridRangeBrushMode::Centre)
					continue;
				FGridRangeCellInfo newInfo;
				newInfo.GridPos = gridPos - CentrePoint;
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
void UGridRangeMakerWindow::OnGridTextYChange(const FText& NewText)
{
	FString stringText = NewText.ToString();
	if (!stringText.IsNumeric())
	{
		InputGridSizeX->SetText(FText::GetEmpty());
	}
	else
	{
		if (FCString::Atoi(*stringText) > 50)
			InputGridSizeX->SetText(FText::AsNumber(50));
	}
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
	else if (!stringText.IsNumeric())
	{
		InputDirMag->SetText(FText::GetEmpty());
		if (MagnitudeChanging)
			*MagnitudeChanging = -1;
	}
	else
	{
		if (MagnitudeChanging)
			*MagnitudeChanging = FCString::Atoi(*stringText);
	}
	//*infoPtr = info;
}
#pragma endregion

void UGridRangeMakerWindow::SetUpMouseTrack()
{
	if (!World)
	{
		World = GEditor->GetEditorWorldContext().World();
		TimerManager = &World->GetTimerManager();
	}
	if (!VisualiserObject)
	{
		UnselectCurrentButton();
		return;
	}
	if (!LevelGridVisualISM)
		LevelGridVisualISM = VisualiserObject->GridVisualComp;
	// reset values for new hover
	LastHoverIndex = -1;
	LastClickIndex = -1;

	// start mouse track
	TimerManager->SetTimer(HoverTimerHandle, this, &UGridRangeMakerWindow::TrackMouse, 0.01f, true);
}

void UGridRangeMakerWindow::TrackMouse()
{
	// checks
	if (!World || !VisualiserObject)
	{
		if (!World)
		{
			World = GEditor->GetEditorWorldContext().World();
			TimerManager = &World->GetTimerManager();
		}
		TimerManager->ClearTimer(HoverTimerHandle);
		UnselectCurrentButton();
		CurrentModeSelected = EGridRangeBrushMode::None;
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
			if (clientViewpoint.Viewport->KeyState(EKeys::LeftMouseButton) && LastClickIndex != hitIndex)
			{
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
						CellData->CellType = EGridRangeBrushMode::None;
						CellData->ClearModifier();
						break;
					case EGridRangeBrushMode::Slot:
						CellData->CellType = EGridRangeBrushMode::Slot;
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
						TimerManager->ClearTimer(HoverTimerHandle);
						break;
					case EGridRangeBrushMode::Select:
						if (CellData->CellType != EGridRangeBrushMode::Slot)
							return;
						//info = *CellData;
						infoPtr = CellData;
						DirectionCanvas->SetVisibility(ESlateVisibility::Visible);
						CurrentDirSelected = ECompassDirection::None;
						LoadDirectionsOnCell();
						TimerManager->ClearTimer(HoverTimerHandle);
						return;
					default:
						break;
					}
					ISMC->SetCustomData(hitIndex, GetColour(CellData->CellType), false);
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
	if (TimerManager)
		TimerManager->ClearTimer(HoverTimerHandle);
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
	case EGridRangeBrushMode::Select:
		SelectButton->SetBackgroundColor(FLinearColor::White);
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
void UGridRangeMakerWindow::LoadDirectionsOnCell()
{
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
		InputDirMag->SetIsEnabled(false);
		CurrentDirSelected = ECompassDirection::None;
	}
	else
	{
		InputDirMag->SetIsEnabled(true);
		CurrentDirSelected = dir;
		button->SetBackgroundColor(FLinearColor::Yellow);
		MagnitudeChanging = GetMagnitudeOfDirection();
		if (*MagnitudeChanging == -1)
		{
			InputDirMag->SetText(FText::FromString("N/A"));
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
		default:
			return {1.0f, 1.0f, 1.0f, 1.0f};
	}
}

#endif
