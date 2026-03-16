#include "BattlePlayerEntity.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/EncounterRelatedCode/BattleManager.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/DefaultInstance.h"
#include "Terminal_Recall/StuffForUI/ActionButtonUI.h"

ABattlePlayerEntity::ABattlePlayerEntity()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABattlePlayerEntity::OnBattleStart(ABattleManager* battleManager)
{
	Super::OnBattleStart(battleManager);
	if (UDefaultInstance* gameInstance =  Cast<UDefaultInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		for (auto attackType : gameInstance->PlayerMoves)
		{
			if (UActionButtonUI* newMoveButton =Cast<UActionButtonUI>(CreateWidget(GetWorld()->GetFirstPlayerController(), ActionButtonClass)))
			{
				newMoveButton->Entity = this;
				newMoveButton->ActionType = EActionType::Movement;
				newMoveButton->AttackType = attackType;
				if (attackType != EAttackActionTypes::Custom)
				{
					newMoveButton->ActionName = GetActionName(attackType);
				}
				MovementActionsUI.Add(newMoveButton);
				newMoveButton->OnActionSelected.AddDynamic(battleManager, &ABattleManager::ActionChoiceSelected);
			}
			if (UActionButtonUI* newAttackButton =Cast<UActionButtonUI>(CreateWidget(GetWorld()->GetFirstPlayerController(), ActionButtonClass)))
			{
				newAttackButton->Entity = this;
				newAttackButton->ActionType = EActionType::Attack;
				newAttackButton->AttackType = attackType;
				AttackActionsUI.Add(newAttackButton);
				newAttackButton->OnActionSelected.AddDynamic(battleManager, &ABattleManager::ActionChoiceSelected);
			}
		}
	}
}

FString ABattlePlayerEntity::GetActionName(EAttackActionTypes type)
{
	switch (type)
	{
	case EAttackActionTypes::King:
		return "King";
	case EAttackActionTypes::Bishop:
		return "Bishop";
	case EAttackActionTypes::Knight:
		return "Knight";
	case EAttackActionTypes::Queen:
		return "Queen";
	case EAttackActionTypes::Rook:
		return "Rook";
	default:
	return "";
	}
}

