#include "ActionButtonUI.h"

#include "Components/Button.h"

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