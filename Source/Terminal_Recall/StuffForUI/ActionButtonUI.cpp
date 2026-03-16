#include "ActionButtonUI.h"

#include "Components/Button.h"

void UActionButtonUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (ActionButton)
		ActionButton->OnClicked.AddDynamic(this, &UActionButtonUI::OnButtonClicked);
}

void UActionButtonUI::OnButtonClicked()
{
	UE_LOG(LogTemp, Display, TEXT("STOP CLICKING ME!"));
}
