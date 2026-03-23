#pragma once

#include "CoreMinimal.h"
#include "BattleEntity.h"
#include "BattlePlayerEntity.generated.h"

UCLASS()
class TERMINAL_RECALL_API ABattlePlayerEntity : public ABattleEntity
{
	GENERATED_BODY()
public:
	ABattlePlayerEntity();
	virtual void OnBattleStart(ABattleManager* battleManager, int32 enemyID = -1) override;
};
