#include "GridRangeMakerWindow.h"

#if WITH_EDITOR
#include "EditorUtilityWidgetComponents.h"
#include "Kismet/GameplayStatics.h"
#include "LevelEditor.h"
#include "SLevelViewport.h"
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
			VisualiserObject = GEditor->GetEditorWorldContext().World()->SpawnActor<AGridVisualiser>(VisualiserClass, FVector(-500.0f, -500.0f, 0.0f), FRotator::ZeroRotator, SpawnParams);
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
	}
}

void UGridRangeMakerWindow::OnPaintButtonClicked()
{
}

void UGridRangeMakerWindow::OnNorthButtonClicked()
{
}

void UGridRangeMakerWindow::OnNorthEastButtonClicked()
{
}

void UGridRangeMakerWindow::OnEastButtonClicked()
{
}

void UGridRangeMakerWindow::OnSouthEastButtonClicked()
{
}

void UGridRangeMakerWindow::OnSouthButtonClicked()
{
}

void UGridRangeMakerWindow::OnSouthWestButtonClicked()
{
}

void UGridRangeMakerWindow::OnWestButtonClicked()
{
}

void UGridRangeMakerWindow::OnNorthWestButtonClicked()
{
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
	if (!stringText.IsNumeric())
	{
		InputGridSizeY->SetText(FText::GetEmpty());
	}
	else
	{
		if (FCString::Atoi(*stringText) > 50)
			InputGridSizeY->SetText(FText::AsNumber(50));
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
				if (FGridRangeCellInfo* CellData = LevelGridData.Find(LastHoverPos))
				{
					LevelGridVisualISM->SetCustomData(LastHoverIndex, GetColour(CellData->IsPainted), false);
				}
				LastHoverIndex = -1;
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
						ISMC->SetCustomData(LastHoverIndex, GetColour(CellData->IsPainted), false);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Unable to find (%d, %d)"), LastHoverPos.X, LastHoverPos.Y);
					}
				}
				LastHoverIndex = hitIndex;
				LastHoverPos = gridPos;
				ISMC->SetCustomData(hitIndex, { 1.0f, 0.8f, 0.0f, 1.0f }, false);
			}
			if (clientViewpoint.Viewport->KeyState(EKeys::LeftMouseButton) && LastClickIndex != hitIndex)
			{
				LastClickIndex = hitIndex;
				
				if (FGridRangeCellInfo* CellData = LevelGridData.Find(LastHoverPos))
				{
					switch (CurrentModeSelected)
					{
						case EGridRangeBrushMode::Erase:
						CellData->IsPainted = false;
						break;
						case EGridRangeBrushMode::Slot:
						CellData->IsPainted = true;
						break;
						default:
						break;
					}
					ISMC->SetCustomData(hitIndex, GetColour(CellData->IsPainted), false);
				}
			}
			
			UE_LOG(LogTemp, Warning, TEXT("Hovered ISM Pos: (%d, %d)"), gridPos.X, gridPos.Y);
		}
		else
		{
			if (LastHoverIndex != -1)
			{
				if (FGridRangeCellInfo* CellData = LevelGridData.Find(LastHoverPos))
				{
					LevelGridVisualISM->SetCustomData(LastHoverIndex, GetColour(CellData->IsPainted), false);
				}
				LastHoverIndex = -1;
			}
		}
	}
	else
	{
		if (LastHoverIndex != -1)
		{
			if (FGridRangeCellInfo* CellData = LevelGridData.Find(LastHoverPos))
			{
				LevelGridVisualISM->SetCustomData(LastHoverIndex, GetColour(CellData->IsPainted), false);
			}
			LastHoverIndex = -1;
	
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
	case EGridRangeBrushMode::N:
		NorthButton->SetBackgroundColor(FLinearColor::White);
		break;
	case EGridRangeBrushMode::NE:
		NorthEastButton->SetBackgroundColor(FLinearColor::White);
		break;
	case EGridRangeBrushMode::E:
		EastButton->SetBackgroundColor(FLinearColor::White);
		break;
	case EGridRangeBrushMode::SE:
		SouthEastButton->SetBackgroundColor(FLinearColor::White);
		break;
	case EGridRangeBrushMode::S:
		SouthButton->SetBackgroundColor(FLinearColor::White);
		break;
	case EGridRangeBrushMode::SW:
		SouthWestButton->SetBackgroundColor(FLinearColor::White);
		break;
	case EGridRangeBrushMode::W:
		WestButton->SetBackgroundColor(FLinearColor::White);
		break;
	case EGridRangeBrushMode::NW:
		NorthWestButton->SetBackgroundColor(FLinearColor::White);
		break;
	default:
		break;
	}
}

TArray<float> UGridRangeMakerWindow::GetColour(bool isPainted)
{
	if (isPainted)
		return {0.0f, 0.0f, 0.0f, 1.0f};
	return {1.0f, 1.0f, 1.0f, 1.0f};
}

#endif
