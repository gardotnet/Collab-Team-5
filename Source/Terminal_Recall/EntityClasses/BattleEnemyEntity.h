#pragma once

#include "CoreMinimal.h"
#include "BattleEntity.h"
#include "BattleEnemyEntity.generated.h"

class ABaseInteractable;
class ABattleGridCell;
class ABattlePlayerEntity;
class APlayerBattleController;

USTRUCT()
struct FPotentialMoveCellInfo
{
	GENERATED_BODY()
	UPROPERTY()
	ABattleGridCell* Cell = nullptr;
	UPROPERTY()
	TArray<int32> RelatedActionIdInList;
};

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
	TObjectPtr<ABattleGridCell> TargetCell;
	UPROPERTY()
	TArray<ABattleGridCell*> HighlightedCells;
	
	float HeathDropChance = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Stuff For Designers | Necessity")
	TSubclassOf<ABaseInteractable> HealthPickUpClass = nullptr;

public:
	ABattleEnemyEntity();
	virtual void OnTurnGo() override;
	virtual void StartMove(FVector position) override;
	virtual void OnMove() override;
	virtual void OnBattleStart(ABattleManager* battleManager, int32 enemyID) override;
	
	virtual void CustomTakeDamage(float damage) override;

protected:
	UFUNCTION()
	void DelayAttack();
	void SetRandomTargetInList(const TArray<FPotentialMoveCellInfo>& list);
	virtual void OnEntityClicked(AActor* TouchedActor, FKey ButtonPressed) override;
};
