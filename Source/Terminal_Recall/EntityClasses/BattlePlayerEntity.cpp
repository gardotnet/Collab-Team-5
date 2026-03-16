#include "BattlePlayerEntity.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/DefaultInstance.h"
#include "Terminal_Recall/StuffForUI/ActionButtonUI.h"

ABattlePlayerEntity::ABattlePlayerEntity()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABattlePlayerEntity::OnBattleStart()
{
	Super::OnBattleStart();
	// transfer stuff from overworld 
}

void ABattlePlayerEntity::BeginPlay()
{
	Super::BeginPlay();
	if (UDefaultInstance* gameInstance =  Cast<UDefaultInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		for (auto attackType : gameInstance->PlayerMoves)
		{
			if (UActionButtonUI* newAttackButton =Cast<UActionButtonUI>(CreateWidget(GetWorld()->GetFirstPlayerController(), ActionButtonClass)))
			{
				newAttackButton->ActionType = EActionType::Attack;
				newAttackButton->AttackType = attackType;
				AttackActionsUI.Add(newAttackButton);
			}
		}
	}
}
