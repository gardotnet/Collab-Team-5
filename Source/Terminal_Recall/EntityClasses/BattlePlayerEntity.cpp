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
		for (auto attackID : gameInstance->AttackList)
		{
			FString stringConvert = FString::FromInt(attackID);
			if (FActionStruct* attackInfo = ActionDataTable->FindRow<FActionStruct>(FName(*stringConvert), TEXT("Entity Search")))
			{
				auto extractedData = LoadActionInfoFromPath(FPaths::ProjectContentDir() + attackInfo->GridRangeFilePath);
				if (!extractedData.IsSet())
					continue;
				AttackInfoList.Add(extractedData.GetValue());
				if (UActionButtonUI* newAttackButton =Cast<UActionButtonUI>(CreateWidget(GetWorld()->GetFirstPlayerController(), ActionButtonClass)))
				{
					newAttackButton->Entity = this;
					newAttackButton->ActionType = EActionType::Attack;
					newAttackButton->ActionName = attackInfo->ActionName.ToString();
					AttackActionsUI.Add(newAttackButton);
					newAttackButton->IndexInOwningEntityArray = AttackActionsUI.Num() - 1;
					newAttackButton->OnActionSelected.AddDynamic(battleManager, &ABattleManager::ActionChoiceSelected);
				}
			}
		}
		for (auto moveID : gameInstance->MoveList)
		{
			FString stringConvert = FString::FromInt(moveID);
			if (FActionStruct* moveInfo = ActionDataTable->FindRow<FActionStruct>(FName(*stringConvert), TEXT("Entity Search")))
			{
				auto extractedData = LoadActionInfoFromPath(FPaths::ProjectContentDir() + moveInfo->GridRangeFilePath);
				if (!extractedData.IsSet())
					continue;
				MoveInfoList.Add(extractedData.GetValue());
				if (UActionButtonUI* newMoveButton =Cast<UActionButtonUI>(CreateWidget(GetWorld()->GetFirstPlayerController(), ActionButtonClass)))
				{
					newMoveButton->Entity = this;
					newMoveButton->ActionType = EActionType::Movement;
					newMoveButton->ActionName = moveInfo->ActionName.ToString();
					MovementActionsUI.Add(newMoveButton);
					newMoveButton->IndexInOwningEntityArray = MovementActionsUI.Num() - 1;
					newMoveButton->OnActionSelected.AddDynamic(battleManager, &ABattleManager::ActionChoiceSelected);
				}
			}
		}
	}
}

