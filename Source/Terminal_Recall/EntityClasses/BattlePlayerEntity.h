#pragma once

#include "CoreMinimal.h"
#include "BattleEntity.h"
#include "Terminal_Recall/StructsAndEnums/ActionTypes.h"
#include "BattlePlayerEntity.generated.h"

UCLASS()
class TERMINAL_RECALL_API ABattlePlayerEntity : public ABattleEntity
{
	GENERATED_BODY()

public:
	ABattlePlayerEntity();
	virtual void OnBattleStart(ABattleManager* battleManager) override;;

protected:
	FString GetActionName(EAttackActionTypes type);
};
