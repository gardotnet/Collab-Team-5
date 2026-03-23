#pragma once

#include "CoreMinimal.h"
#include "BattleEntity.h"
#include "BattleEnemyEntity.generated.h"

class ABattleGridCell;
class ABattlePlayerEntity;
class APlayerBattleController;

UCLASS()
class TERMINAL_RECALL_API ABattleEnemyEntity : public ABattleEntity
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Stuff For Designers | Necessities")
	TObjectPtr<UDataTable> EnemyDataTable;
protected:
	UPROPERTY()
	TObjectPtr<UWorld> WorldRef;
	UPROPERTY()
	TObjectPtr<ABattlePlayerEntity> PlayerRef;
	UPROPERTY()
	int32 EnemyMoveRange;
	UPROPERTY()
	TObjectPtr<ABattleGridCell> TargetCell;
public:
	ABattleEnemyEntity();
	virtual void OnTurnGo() override;
	virtual void StartMove(FVector position) override;
	virtual void OnBattleStart(ABattleManager* battleManager, int32 enemyID) override;
};
