#pragma once

#include "CoreMinimal.h"
#include "BattleEntity.h"
#include "BattleEnemyEntity.generated.h"

class APlayerBattleController;

UCLASS()
class TERMINAL_RECALL_API ABattleEnemyEntity : public ABattleEntity
{
	GENERATED_BODY()

public:
	ABattleEnemyEntity();
	
	virtual void OnTurnGo() override;
};
