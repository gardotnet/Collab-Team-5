#include "MainBattleUI.h"

#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

void UMainBattleUI::ChangeHealth(int32 newHealthValue)
{
	HealthText->SetText(FText::AsNumber(newHealthValue));
}

void UMainBattleUI::SetEnemyHp(int32 newHealthValue)
{
	EnemyHealthText->SetText(FText::AsNumber(newHealthValue));
	EnemyHPContainer->SetVisibility(ESlateVisibility::Visible);
}

void UMainBattleUI::HideEnemyHp()
{
	EnemyHPContainer->SetVisibility(ESlateVisibility::Hidden);
}


