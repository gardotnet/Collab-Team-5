#include "GridRangeMakerWindow.h"

#if WITH_EDITOR
#include "EditorUtilityWidgetComponents.h"
#include "Kismet/GameplayStatics.h"
#include "LevelEditor.h"
#include "SLevelViewport.h"
#include "Components/CanvasPanel.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Terminal_Recall/StructsAndEnums/WorldNeighbourStruct.h"

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
	if (UndoButton)
		UndoButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::OnUndoButtonClicked);
	if (RedoButton)
		RedoButton->OnClicked.AddDynamic(this, &UGridRangeMakerWindow::OnRedoButtonClicked);
	
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
			VisualiserObject = GEditor->GetEditorWorldContext().World()->SpawnActor<AGridVisualiser>(VisualiserClass, FVector(-550.0f, -550.0f, 0.0f), FRotator::ZeroRotator, SpawnParams);
		}
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
	DirectionCanvas->SetVisibility(ESlateVisibility::Hidden);
}

void UGridRangeMakerWindow::OnNorthButtonClicked()
{
	UnselectDirectionButton();
	if (CurrentDirSelected == ECompassDirection::N)
	{
		CurrentDirSelected = ECompassDirection::None;
	}
	else
	{
		NorthButton->SetBackgroundColor(FLinearColor::Yellow);
		CurrentDirSelected = ECompassDirection::N;
	}
}
void UGridRangeMakerWindow::OnNorthEastButtonClicked()
{
	UnselectDirectionButton();
	if (CurrentDirSelected == ECompassDirection::NE)
	{
		CurrentDirSelected = ECompassDirection::None;
	}
	else
	{
		CurrentDirSelected = ECompassDirection::NE;
		NorthEastButton->SetBackgroundColor(FLinearColor::Yellow);
	}
}
void UGridRangeMakerWindow::OnEastButtonClicked()
{
	UnselectDirectionButton();
	if (CurrentDirSelected == ECompassDirection::E)
	{
		CurrentDirSelected = ECompassDirection::None;
	}
	else
	{
		CurrentDirSelected = ECompassDirection::E;
		EastButton->SetBackgroundColor(FLinearColor::Yellow);
	}
}
void UGridRangeMakerWindow::OnSouthEastButtonClicked()
{
	UnselectDirectionButton();
	if (CurrentDirSelected == ECompassDirection::SE)
	{
		CurrentDirSelected = ECompassDirection::None;
	}
	else
	{
		CurrentDirSelected = ECompassDirection::SE;
		SouthEastButton->SetBackgroundColor(FLinearColor::Yellow);
	}
}
void UGridRangeMakerWindow::OnSouthButtonClicked()
{
	UnselectDirectionButton();
	if (CurrentDirSelected == ECompassDirection::S)
	{
		CurrentDirSelected = ECompassDirection::None;
	}
	else
	{
		CurrentDirSelected = ECompassDirection::S;
		SouthButton->SetBackgroundColor(FLinearColor::Yellow);
	}
}
void UGridRangeMakerWindow::OnSouthWestButtonClicked()
{
	UnselectDirectionButton();
	if (CurrentDirSelected == ECompassDirection::SW)
	{
		CurrentDirSelected = ECompassDirection::None;
	}
	else
	{
		CurrentDirSelected = ECompassDirection::SW;
		SouthWestButton->SetBackgroundColor(FLinearColor::Yellow);
	}
}
void UGridRangeMakerWindow::OnWestButtonClicked()
{
	UnselectDirectionButton();
	if (CurrentDirSelected == ECompassDirection::W)
	{
		CurrentDirSelected = ECompassDirection::None;
	}
	else
	{
		CurrentDirSelected = ECompassDirection::W;
		WestButton->SetBackgroundColor(FLinearColor::Yellow);
	}
}
void UGridRangeMakerWindow::OnNorthWestButtonClicked()
{
	UnselectDirectionButton();
	if (CurrentDirSelected == ECompassDirection::NW)
	{
		CurrentDirSelected = ECompassDirection::None;
	}
	else
	{
		CurrentDirSelected = ECompassDirection::NW;
		NorthWestButton->SetBackgroundColor(FLinearColor::Yellow);
	}
}

void UGridRangeMakerWindow::OnLoadLevelFileButtonClicked()
{
}

void UGridRangeMakerWindow::OnExportButtonClicked()
{
}

void UGridRangeMakerWindow::OnUndoButtonClicked()
{
}

void UGridRangeMakerWindow::OnRedoButtonClicked()
{
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
	}
	if (!stringText.IsNumeric())
	{
		InputDirMag->SetText(FText::GetEmpty());
	}
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
					if (FGridRangeCellInfo* CellData = LevelGridData.Find(LastHoverPos))
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
				LastClickIndex = hitIndex;
				if (FGridRangeCellInfo* CellData = LevelGridData.Find(LastHoverPos))
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
						break;
						case EGridRangeBrushMode::Slot:
						CellData->CellType = EGridRangeBrushMode::Slot;
						break;
						case EGridRangeBrushMode::Centre:
							CellData->CellType = EGridRangeBrushMode::Centre;
							CentrePlaced = true;
							CentrePoint = gridPos;
							CentreButton->SetIsEnabled(false);
							UnselectCurrentButton();
							TimerManager->ClearTimer(HoverTimerHandle);
						break;
						case EGridRangeBrushMode::Select:
							SelectedCellInfo = CellData;
							DirectionCanvas->SetVisibility(ESlateVisibility::Visible);
							CurrentDirSelected = ECompassDirection::None;
							LoadDirectionsOnCell();
							UnselectCurrentButton();
							TimerManager->ClearTimer(HoverTimerHandle);
							return;
						default:
						break;
					}
					ISMC->SetCustomData(hitIndex, GetColour(CellData->CellType), false);
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("Hovered ISM Pos: (%d, %d)"), gridPos.X, gridPos.Y);
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
	/*switch (CurrentDirSelected)
	{
		case ECompassDirection::N:
			NorthButton->SetBackgroundColor(GetDirButtonColor(ECompassDirection::N));
		break;
		case ECompassDirection::NE:
			NorthEastButton->SetBackgroundColor(GetDirButtonColor(ECompassDirection::NE));
		break;
		case ECompassDirection::E:
			EastButton->SetBackgroundColor(GetDirButtonColor(ECompassDirection::E));
		break;
		case ECompassDirection::SE:
			SouthEastButton->SetBackgroundColor(GetDirButtonColor(ECompassDirection::SE));
		break;
		case ECompassDirection::S:
			SouthButton->SetBackgroundColor(GetDirButtonColor(ECompassDirection::S));
		break;
		case ECompassDirection::SW:
			SouthWestButton->SetBackgroundColor(GetDirButtonColor(ECompassDirection::SW));
		break;
		case ECompassDirection::W:
			WestButton->SetBackgroundColor(GetDirButtonColor(ECompassDirection::W));
		break;
		case ECompassDirection::NW:
			NorthWestButton->SetBackgroundColor(GetDirButtonColor(ECompassDirection::NW));
		break;
		default:
		break;
	}*/
}

void UGridRangeMakerWindow::UnselectAllDirectionButton()
{
	NorthButton->SetBackgroundColor(FLinearColor::White);
	NorthEastButton->SetBackgroundColor(FLinearColor::White);
	EastButton->SetBackgroundColor(FLinearColor::White);
	SouthEastButton->SetBackgroundColor(FLinearColor::White);
	SouthButton->SetBackgroundColor(FLinearColor::White);
	SouthWestButton->SetBackgroundColor(FLinearColor::White);
	WestButton->SetBackgroundColor(FLinearColor::White);
	NorthWestButton->SetBackgroundColor(FLinearColor::White);
}

void UGridRangeMakerWindow::LoadDirectionsOnCell()
{
	UnselectAllDirectionButton();
	
		/*
		switch (direction.Direction)
		{
			case ECompassDirection::N:
			NorthButton->SetBackgroundColor(FLinearColor::Green);
			break;
			case ECompassDirection::NE:
			NorthEastButton->SetBackgroundColor(FLinearColor::Green);
			break;
			case ECompassDirection::E:
			EastButton->SetBackgroundColor(FLinearColor::Green);
			break;
			case ECompassDirection::SE:
			SouthEastButton->SetBackgroundColor(FLinearColor::Green);
			case ECompassDirection::S:
			SouthButton->SetBackgroundColor(FLinearColor::Green);
			break;
			case ECompassDirection::SW:
			SouthWestButton->SetBackgroundColor(FLinearColor::Green);
			case ECompassDirection::W:
			WestButton->SetBackgroundColor(FLinearColor::Green);
			break;
			case ECompassDirection::NW:
			NorthWestButton->SetBackgroundColor(FLinearColor::Green);
			break;
			default:
			break;
		}*/
		
	//}
}


void UGridRangeMakerWindow::UnselectCell()
{
	if (FGridRangeCellInfo* CellData = LevelGridData.Find(LastHoverPos))
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
