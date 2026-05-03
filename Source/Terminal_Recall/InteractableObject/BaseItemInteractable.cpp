#include "BaseItemInteractable.h"
#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/DefaultInstance.h"

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
	UDefaultInstance* GI = Cast<UDefaultInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GI) return;

	// add attack to player's unlocked mech part array
	if (AttackIndex >= 0 && !GI->AttackList.Contains(AttackIndex))
	{
		GI->AttackList.Add(AttackIndex);
	}

	// add move to player's unlocked mech part array
	if (MoveIndex >= 0 && !GI->MoveList.Contains(MoveIndex))
	{
		GI->MoveList.Add(MoveIndex);
	}

	RemoveFromGame();
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
