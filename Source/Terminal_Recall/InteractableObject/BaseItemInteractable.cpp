#include "BaseItemInteractable.h"
#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/DefaultInstance.h"
#include "Terminal_Recall/EncounterRelatedCode/BattleManager.h"

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

	/* ATTACK / MOVE ASSIGNMENT TO PLAYER ON MECH PART PICKUP */

	// add attack to player's attack array
	if (AttackIndex >= 0 && GI->AttackList.IsValidIndex(AttackIndex)){
		if (GI->AttackList[AttackIndex] == INDEX_NONE){
			GI->AttackList[AttackIndex] = AttackIndex;
			UE_LOG(LogTemp, Warning, TEXT("[ITEM PICKUP] attack added at index %d"), AttackIndex);
		}
	}

	// add move to player's move array
	if (MoveIndex >= 0 && GI->MoveList.IsValidIndex(MoveIndex)){
		if (GI->MoveList[MoveIndex] == INDEX_NONE){
			GI->MoveList[MoveIndex] = MoveIndex;
			UE_LOG(LogTemp, Warning, TEXT("[ITEM PICKUP] move added at index %d"), MoveIndex);
		}
	}

	RemoveFromGame();
	if (ABattleManager* BM = Cast<ABattleManager>(UGameplayStatics::GetActorOfClass(this, ABattleManager::StaticClass())))
	{
		BM->EndCombat();
	}
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
