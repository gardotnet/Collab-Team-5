#include "BattleChoiceSelectUI.h"

#include "ActionButtonUI.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"

void UBattleChoiceSelectUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (CloseButton)
		CloseButton->OnClicked.AddDynamic(this, &UBattleChoiceSelectUI::OnCloseButtonClicked);
	if (MoveTabButton)
		MoveTabButton->OnClicked.AddDynamic(this, &UBattleChoiceSelectUI::OnMoveTabButtonClicked);
	if (AttackTabButton)
		AttackTabButton->OnClicked.AddDynamic(this, &UBattleChoiceSelectUI::OnAttackTabButtonClicked);
	if (ItemTabButton)
		ItemTabButton->OnClicked.AddDynamic(this, &UBattleChoiceSelectUI::OnItemTabButtonClicked);
}

void UBattleChoiceSelectUI::NativeDestruct()
{
	Super::NativeDestruct();
	if (CloseButton)
		CloseButton->OnClicked.RemoveDynamic(this, &UBattleChoiceSelectUI::OnCloseButtonClicked);
	if (MoveTabButton)
		MoveTabButton->OnClicked.RemoveDynamic(this, &UBattleChoiceSelectUI::OnMoveTabButtonClicked);
	if (AttackTabButton)
		AttackTabButton->OnClicked.RemoveDynamic(this, &UBattleChoiceSelectUI::OnAttackTabButtonClicked);
	if (ItemTabButton)
		ItemTabButton->OnClicked.RemoveDynamic(this, &UBattleChoiceSelectUI::OnItemTabButtonClicked);
}

void UBattleChoiceSelectUI::OnCloseButtonClicked()
{
	RemoveFromParent();
}

void UBattleChoiceSelectUI::OnMoveTabButtonClicked()
{
	UnselectTab();
	MoveTabButton->SetBackgroundColor(SelectColour);
	MovesScrollBox->SetVisibility(ESlateVisibility::Visible);
}

void UBattleChoiceSelectUI::OnAttackTabButtonClicked()
{
	UnselectTab();
	AttackTabButton->SetBackgroundColor(SelectColour);
	AttacksScrollBox->SetVisibility(ESlateVisibility::Visible);
}

void UBattleChoiceSelectUI::OnItemTabButtonClicked()
{
	UnselectTab();
	ItemTabButton->SetBackgroundColor(SelectColour);
	ItemsScrollBox->SetVisibility(ESlateVisibility::Visible);
}

void UBattleChoiceSelectUI::UnselectTab()
{
	MoveTabButton->SetBackgroundColor(UnselectColour);
	AttackTabButton->SetBackgroundColor(UnselectColour);
	ItemTabButton->SetBackgroundColor(UnselectColour);
	MovesScrollBox->SetVisibility(ESlateVisibility::Hidden);
	AttacksScrollBox->SetVisibility(ESlateVisibility::Hidden);
	ItemsScrollBox->SetVisibility(ESlateVisibility::Hidden);
}

void UBattleChoiceSelectUI::RemoveChoices()
{
	for (int i = 0; i < MovesScrollBox->GetChildrenCount(); i++)
	{
		MovesScrollBox->RemoveChild(MovesScrollBox->GetChildAt(0));
	}
	for (int i = 0; i < AttacksScrollBox->GetChildrenCount(); i++)
	{
		AttacksScrollBox->RemoveChild(AttacksScrollBox->GetChildAt(0));
	}
	for (int i = 0; i < ItemsScrollBox->GetChildrenCount(); i++)
	{
		ItemsScrollBox->RemoveChild(ItemsScrollBox->GetChildAt(0));
	}
}

void UBattleChoiceSelectUI::AddActionButton(TArray<UActionButtonUI*>& movementList,
                                            TArray<UActionButtonUI*>& attackList, TArray<UActionButtonUI*>& itemList)
{
	AddToViewport();
	RemoveChoices();
	if (movementList.Num() > 0)
	{
		for (UActionButtonUI* moveAction : movementList)
		{
			if (UUserWidget* parentConversion = Cast<UUserWidget>(moveAction))
				MovesScrollBox->AddChild(parentConversion);
		}
	}
	if (attackList.Num() > 0)
	{
		for (UActionButtonUI* attackAction : attackList)
		{
			if (UUserWidget* parentConversion = Cast<UUserWidget>(attackAction))
				AttacksScrollBox->AddChild(parentConversion);
		}
	}
	if (itemList.Num() > 0)
	{
		for (UActionButtonUI* itemAction : itemList)
		{
			if (UUserWidget* parentConversion = Cast<UUserWidget>(itemAction))
				ItemsScrollBox->AddChild(parentConversion);
		}
	}
}




