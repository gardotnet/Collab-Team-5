#if WITH_EDITOR
#include "EncounterLevelMakerWindow.h"

#include "EditorUtilityWidgetComponents.h"
#include "EncounterLevelMakerManager.h"
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
}

#pragma region Button Functions

void UEncounterLevelMakerWindow::OnCreateGridPressed()
{
	if (!MakerManager)
		MakerManager = Cast<AEncounterLevelMakerManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AEncounterLevelMakerManager::StaticClass()));
	
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
		if (EnemyComboBox->GetSelectedIndex() == 0)
		{
			CurrentModeSelected = ECellBrushMode::None;
			HighlightComboBox(TrapComboBox);
		}
		else
		{
			CurrentModeSelected = ECellBrushMode::Trap;
			TrapToggleButton->SetBackgroundColor(FLinearColor::Green);
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

void UEncounterLevelMakerWindow::OnGridTextXChange(const FText& NewText)
{
	FString stringText = NewText.ToString();
	if (stringText.IsNumeric())
	{
		InputGridSizeX->SetText(FText::GetEmpty());
	}
	else
	{
		if (FCString::Atoi(*stringText) < 100)
			InputGridSizeX->SetText(FText::AsNumber(100));
	}
}

void UEncounterLevelMakerWindow::OnGridTextYChange(const FText& NewText)
{
	FString stringText = NewText.ToString();
	if (stringText.IsNumeric())
	{
		InputGridSizeY->SetText(FText::GetEmpty());
	}
	else
	{
		if (FCString::Atoi(*stringText) < 100)
			InputGridSizeY->SetText(FText::AsNumber(100));
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

#pragma region Utilities

void UEncounterLevelMakerWindow::UnselectCurrent()
{
	UnhighlightComboBox(EnemyComboBox);
	UnhighlightComboBox(TrapComboBox);
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

#pragma endregion

#endif