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

void ABattlePlayerEntity::OnBattleStart(ABattleManager* battleManager, float health, float damageMultiplier)
{
	BattleManager = battleManager;
	if (UDefaultInstance* gameInstance =  Cast<UDefaultInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		/*for (auto attackType : gameInstance->PlayerMoves)
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
		}*/
		for (auto attackID : gameInstance->AttackList)
		{
			FString stringConvert = FString::FromInt(attackID);
			if (FAttackStruct* attackInfo = AttackDataTable->FindRow<FAttackStruct>(FName(*stringConvert), TEXT("Entity Search")))
			{
				AttackInfoList.Add(attackInfo);
				if (UActionButtonUI* newAttackButton =Cast<UActionButtonUI>(CreateWidget(GetWorld()->GetFirstPlayerController(), ActionButtonClass)))
				{
					newAttackButton->Entity = this;
					newAttackButton->ActionType = EActionType::Attack;
					AttackActionsUI.Add(newAttackButton);
					newAttackButton->IndexInOwningEntityArray = AttackActionsUI.Num() - 1;
					newAttackButton->OnActionSelected.AddDynamic(battleManager, &ABattleManager::ActionChoiceSelected);
				}
			}
		}
		
		// Temp Attack and move
		if (UActionButtonUI* newAttackButton =Cast<UActionButtonUI>(CreateWidget(GetWorld()->GetFirstPlayerController(), ActionButtonClass)))
		{
			newAttackButton->Entity = this;
			newAttackButton->ActionType = EActionType::Attack;
			AttackActionsUI.Add(newAttackButton);
			newAttackButton->IndexInOwningEntityArray = AttackActionsUI.Num() - 1;
			newAttackButton->OnActionSelected.AddDynamic(battleManager, &ABattleManager::ActionChoiceSelected);
		}
		
		// Temp Attack and move
		if (UActionButtonUI* newMoveButton =Cast<UActionButtonUI>(CreateWidget(GetWorld()->GetFirstPlayerController(), ActionButtonClass)))
		{
			newMoveButton->Entity = this;
			newMoveButton->ActionType = EActionType::Movement;
			MovementActionsUI.Add(newMoveButton);
			newMoveButton->IndexInOwningEntityArray = MovementActionsUI.Num() - 1;
			newMoveButton->OnActionSelected.AddDynamic(battleManager, &ABattleManager::ActionChoiceSelected);
		}
	}
}

