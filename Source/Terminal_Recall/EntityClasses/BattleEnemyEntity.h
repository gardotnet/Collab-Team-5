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
	UPROPERTY(EditDefaultsOnly, Category = "Stuff For Designers | Necessities")
	TObjectPtr<UDataTable> EnemyDataTable;
	
	TArray<FIntPoint> AttackRange;
	
	ABattleEnemyEntity();

	virtual void OnTurnGo() override;
	virtual void OnBattleStart(ABattleManager* battleManager, int32 enemyID) override;
};
