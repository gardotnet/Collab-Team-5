#include "ActionButtonUI.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UActionButtonUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (ActionButton)
		ActionButton->OnClicked.AddDynamic(this, &UActionButtonUI::OnButtonClicked);
}
void UActionButtonUI::NativeDestruct()
{
	Super::NativeDestruct();
	if (ActionButton)
		ActionButton->OnClicked.RemoveDynamic(this, &UActionButtonUI::OnButtonClicked);
}
void UActionButtonUI::OnButtonClicked()
{
	OnActionSelected.Broadcast(this);
}

void UActionButtonUI::RemoveForChoices()
{
	RemoveFromParent();
}

void UActionButtonUI::SetCooldown(int32 cooldown)
{
	Cooldown = cooldown;
	if (cooldown == 0)
	{
		NameLabel->SetText(FText::FromString(ActionName));
		ActionButton->SetIsEnabled(true);
		return;
	}
	NameLabel->SetText(FText::AsNumber(cooldown));
	ActionButton->SetIsEnabled(false);
	
}
