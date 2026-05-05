#include "BaseItemInteractable.h"

ABaseItemInteractable::ABaseItemInteractable()
{
	ItemName = TEXT("Default Item");
	ItemDescription = TEXT("Default Description");
}

void ABaseItemInteractable::ShowConfirmationUI_Implementation()
{
}

void ABaseItemInteractable::OnItemUse_Implementation()
{
}

void ABaseItemInteractable::GetItemInfo(FString& OutName, FString& OutDescription) const
{
	OutName = ItemName;
	OutDescription = ItemDescription;
}

void ABaseItemInteractable::OnActivate_Implementation(ABattleEntity* entity)
{
	Super::OnActivate_Implementation(entity);
}
