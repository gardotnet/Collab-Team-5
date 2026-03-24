#include "LevelSelectWindow.h"

#include "LevelSelectIconWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/StructsAndEnums/ForDataTable/EnemyInfoStruct.h"

void ULevelSelectWindow::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (CloseButton)
		CloseButton->OnClicked.AddDynamic(this, &ULevelSelectWindow::OnCloseButtonPressed);
	if (EnterLevelButton)
		EnterLevelButton->OnClicked.AddDynamic(this, &ULevelSelectWindow::OnEnterLevelButtonPressed);
	
	for (int i = 0; i < 5; i++)
	{
		ULevelSelectIconWidget* iconWidget = Cast<ULevelSelectIconWidget>(CreateWidget(GetWorld()->GetFirstPlayerController(), IconWidgetClass));
		iconWidget->IconImage->SetBrushFromTexture(DifficultIconTexture);
		DifficultWidgetHolder.Add(iconWidget);
	}
	if (EnemyDataTable)
	{
		TArray<FName> rowNames = EnemyDataTable->GetRowNames();
		for (const FName& name : rowNames)
		{
			if (FEnemyInfoStruct* enemyInfo = EnemyDataTable->FindRow<FEnemyInfoStruct>(name, TEXT("Encounter Tool")))
			{
				ULevelSelectIconWidget* iconWidget = Cast<ULevelSelectIconWidget>(CreateWidget(GetWorld()->GetFirstPlayerController(), IconWidgetClass));
				iconWidget->IconImage->SetBrushFromTexture(enemyInfo->EnemyIconTexture);
				EnemyIconWidgetHolder.Add(iconWidget);
			}
		}
	}
}

void ULevelSelectWindow::NativeDestruct()
{
	Super::NativeDestruct();
	if (CloseButton)
		CloseButton->OnClicked.RemoveDynamic(this, &ULevelSelectWindow::OnCloseButtonPressed);
	if (EnterLevelButton)
		EnterLevelButton->OnClicked.RemoveDynamic(this, &ULevelSelectWindow::OnEnterLevelButtonPressed);
}

void ULevelSelectWindow::OnCloseButtonPressed()
{
	RemoveFromParent();
	ClearUIEvent.Broadcast();
	ClearUIEvent.Clear();
}
void ULevelSelectWindow::OnEnterLevelButtonPressed()
{
	UGameplayStatics::OpenLevel(GetWorld(), LevelName);
}
void ULevelSelectWindow::PassInfoToLevelSelectWindow(int32 difficulty, const TArray<int32>& enemiesID)
{
	ClearUIEvent.Broadcast();
	ClearUIEvent.Clear();
	
	for (auto id : enemiesID)
	{
		ClearUIEvent.AddDynamic(EnemyIconWidgetHolder[id], &ULevelSelectIconWidget::HideUI);
		EnemyHintContainer->AddChild(EnemyIconWidgetHolder[id]);
	}
	
	int difCount = DifficultWidgetHolder.Num() - difficulty;
	if (difCount >= 0)
	{
		for (int i = 0; i < difficulty; i++)
		{
			ClearUIEvent.AddDynamic(DifficultWidgetHolder[i], &ULevelSelectIconWidget::HideUI);
			DifficultyContainer->AddChild(DifficultWidgetHolder[i]);
		}
		return;
	}
	
	for (int i = 0; i < DifficultWidgetHolder.Num(); i++)
	{
		ClearUIEvent.AddDynamic(DifficultWidgetHolder[i], &ULevelSelectIconWidget::HideUI);
		DifficultyContainer->AddChild(DifficultWidgetHolder[i]);
	}
	difCount = difficulty - DifficultWidgetHolder.Num(); 
	for (int i = 0; i < difCount; i++)
	{
		ULevelSelectIconWidget* iconWidget = Cast<ULevelSelectIconWidget>(CreateWidget(GetWorld()->GetFirstPlayerController(), IconWidgetClass));
		iconWidget->IconImage->SetBrushFromTexture(DifficultIconTexture);
		DifficultWidgetHolder.Add(iconWidget);
		DifficultyContainer->AddChild(iconWidget);
	}
}
