#if WITH_EDITOR
#include "EncounterLevelMakerWindow.h"

#include "EditorUtilityWidgetComponents.h"
#include "EncounterLevelMakerManager.h"
#include "LevelEditor.h"
#include "SLevelViewport.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/StructsAndEnums/ForDataTable/EnemyInfoStruct.h"

void UEncounterLevelMakerWindow::NativeConstruct()
{
	Super::NativeConstruct();
	
	#pragma region Links
	
	if (CreateGridButton)
		CreateGridButton->OnClicked.AddDynamic(this, &UEncounterLevelMakerWindow::OnCreateGridPressed);
	if (WalkToggleButton)
		WalkToggleButton->OnClicked.AddDynamic(this, &UEncounterLevelMakerWindow::OnWalkTogglePressed);
	if (WallToggleButton)
		WallToggleButton->OnClicked.AddDynamic(this, &UEncounterLevelMakerWindow::OnWallTogglePressed);
	if (EnemyToggleButton)
		EnemyToggleButton->OnClicked.AddDynamic(this, &UEncounterLevelMakerWindow::OnEnemyTogglePressed);
	if (TrapToggleButton)
		TrapToggleButton->OnClicked.AddDynamic(this, &UEncounterLevelMakerWindow::OnTrapTogglePressed);
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
	
	#pragma endregion 
	
	if (EnemyComboBox && EnemyDataTable)
	{
		TArray<FName> rowNames = EnemyDataTable->GetRowNames();
		
		FEnemyInfoStruct* enemyInfo = nullptr;

		for (FName& name : rowNames)
		{
			enemyInfo = EnemyDataTable->FindRow<FEnemyInfoStruct>(name, TEXT("Encounter Tool"));
			if (enemyInfo)
				EnemyComboBox->AddOption(enemyInfo->EnemyName.ToString());
		}
	}
	HoverTimerHandle = FTimerHandle();
}

#pragma region Button Functions

void UEncounterLevelMakerWindow::OnCreateGridPressed()
{
	if (!MakerManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("Maker manager not exist"))
		MakerManager = Cast<AEncounterLevelMakerManager>(UGameplayStatics::GetActorOfClass(GEditor->GetEditorWorldContext().World(), AEncounterLevelMakerManager::StaticClass()));
		if (!MakerManager)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			MakerManager = GEditor->GetEditorWorldContext().World()->SpawnActor<AEncounterLevelMakerManager>(MakerManagerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		}
	}
	
	FString xString =  InputGridSizeX->GetText().ToString();
	FString yString =  InputGridSizeY->GetText().ToString();
	if (xString == "" || yString == "")
		return;
	int32 x = FCString::Atoi(*xString);
	int32 y = FCString::Atoi(*yString);
	
	MakerManager->CreateGrid(FIntPoint(x, y));
}

void UEncounterLevelMakerWindow::OnWalkTogglePressed()
{
	UnselectCurrent();
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
	UnselectCurrent();
	if (CurrentModeSelected == ECellBrushMode::Wall)
	{
		CurrentModeSelected = ECellBrushMode::None;
	}
	else
	{
		CurrentModeSelected = ECellBrushMode::Wall;
		WallToggleButton->SetBackgroundColor(FLinearColor::Green);
		SetUpTrack();
	}
}

void UEncounterLevelMakerWindow::OnEnemyTogglePressed()
{
	UnselectCurrent();
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
	UnselectCurrent();
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

void UEncounterLevelMakerWindow::OnLoadFilePressed()
{
	UE_LOG(LogTemp, Warning, TEXT("YES LEAK ALL YOUR FILES TO ME!!!"));
}

void UEncounterLevelMakerWindow::OnExportPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Here take a virus"));
}

void UEncounterLevelMakerWindow::OnUndoPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("You wish"));
}

void UEncounterLevelMakerWindow::OnRedoPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Do it again, I dArE yOu"));
}

#pragma endregion

#pragma region On Changes Functions

void UEncounterLevelMakerWindow::OnGridTextXChange(const FText& NewText)
{
	FString stringText = NewText.ToString();
	if (!stringText.IsNumeric())
	{
		InputGridSizeX->SetText(FText::GetEmpty());
	}
	else
	{
		int32 num = FCString::Atoi(*stringText);
		if (num > 100)
		{
			num = 100;
			InputGridSizeX->SetText(FText::AsNumber(100));
		}
		GridSize.X = num;
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
		int32 num = FCString::Atoi(*stringText);
		if (num > 100)
		{
			num = 100;
			InputGridSizeY->SetText(FText::AsNumber(100));
		}
		GridSize.Y = num;
	}
}

void UEncounterLevelMakerWindow::OnEnemySelectChange(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectedItem == "None" && (ESelectInfo::OnMouseClick == SelectionType || ESelectInfo::OnKeyPress == SelectionType))
	{
		if (CurrentModeSelected != ECellBrushMode::EnemySpawn)
			return;
		UnselectCurrent();
		CurrentModeSelected = ECellBrushMode::None;
		HighlightComboBox(EnemyComboBox);
	}
	else
	{
		UnhighlightComboBox(EnemyComboBox);
	}
}

void UEncounterLevelMakerWindow::OnTrapSelectChange(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectedItem == "None" && (ESelectInfo::OnMouseClick == SelectionType || ESelectInfo::OnKeyPress == SelectionType))
	{
		if (CurrentModeSelected != ECellBrushMode::Trap)
			return;
		UnselectCurrent();
		CurrentModeSelected = ECellBrushMode::None;
		HighlightComboBox(TrapComboBox);
	}
	else
	{
		UnhighlightComboBox(TrapComboBox);
	}
}

#pragma endregion

#pragma region Utilities

void UEncounterLevelMakerWindow::UnselectCurrent()
{
	UnhighlightComboBox(EnemyComboBox);
	UnhighlightComboBox(TrapComboBox);
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
		default: 
		break;
	}
}

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

void UEncounterLevelMakerWindow::SetUpTrack()
{
	if (!World)
	{
		World = GEditor->GetEditorWorldContext().World();
		TimerManager = &World->GetTimerManager();
	}
	
	TimerManager->SetTimer(HoverTimerHandle, this, &UEncounterLevelMakerWindow::EnableMouseTrack, 0.01f, true);
}

void UEncounterLevelMakerWindow::EnableMouseTrack()
{
	FLevelEditorModule& LEModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
	TSharedPtr<SLevelViewport> activeViewport = LEModule.GetFirstActiveLevelViewport();

	if (!activeViewport.IsValid())
		return;
	FLevelEditorViewportClient& clientViewpoint = activeViewport->GetLevelViewportClient();
	
	FViewportCursorLocation cursorLoc = clientViewpoint.GetCursorWorldLocationFromMousePos();

	FVector rayStartPos = cursorLoc.GetOrigin();
	FVector rayDir = cursorLoc.GetDirection();
	FVector rayEndPos = rayStartPos + (rayDir * 10000.0f);

	FHitResult hitResult;
	FCollisionQueryParams params;
	params.bTraceComplex = true;

	if (!World)
	{
		World = GEditor->GetEditorWorldContext().World();
		TimerManager = &World->GetTimerManager();
		TimerManager->ClearTimer(HoverTimerHandle);
		UnselectCurrent();
		CurrentModeSelected = ECellBrushMode::None;
	}

	if (World && World->LineTraceSingleByChannel(hitResult, rayStartPos, rayEndPos, ECC_Visibility, params))
	{
		if (UInstancedStaticMeshComponent* ISMC = Cast<UInstancedStaticMeshComponent>(hitResult.GetComponent()))
		{
			int32 hitIndex = hitResult.Item;

			FIntPoint gridPos;
			gridPos.Y = hitIndex / GridSize.X;
			gridPos.X = hitIndex % GridSize.X;
			
			UE_LOG(LogTemp, Warning, TEXT("Hovered ISM Pos: (%d, %d)"), gridPos.X, gridPos.Y);

			if (clientViewpoint.ProcessClick()) 
		}
	}
}

#pragma endregion

#endif