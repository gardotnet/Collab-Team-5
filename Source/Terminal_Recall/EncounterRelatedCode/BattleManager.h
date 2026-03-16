#pragma once

#include "CoreMinimal.h"
#include "Terminal_Recall/StructsAndEnums/ActionType.h"
#include "Terminal_Recall/StructsAndEnums/BattleCellState.h"
#include "Terminal_Recall/StructsAndEnums/For2dArray/FGridRowCell.h"
#include "BattleManager.generated.h"

class ABattleEntity;
class ABattleEnemyEntity;
class ABattlePlayerEntity;

UCLASS()
class TERMINAL_RECALL_API ABattleManager : public AActor
{
	GENERATED_BODY()

public:
	ABattleManager();
	UPROPERTY()
	TObjectPtr<ABattlePlayerEntity> PlayerRef;
	TArray<TObjectPtr<ABattleEnemyEntity>> EnemyArray;
	UPROPERTY()
	TMap<FIntPoint, TObjectPtr<ABattleGridCell>> GridMap;
	
protected:
	int32 EntityIndex = -1;
	UPROPERTY()
	TObjectPtr<ABattleEntity> CurrentEntityChosen;

public:
	void SetUpBattle(TObjectPtr<ABattlePlayerEntity> player, TArray<TObjectPtr<ABattleEnemyEntity>> enemies, TArray<FGridRowCell>* grid);
	void OnEntitySelected(ABattleEntity* entity);
	void ActionChoiceSelected(EBattleCellState attackOrMove, EActionType ActionType);
protected:
	UFUNCTION()
	void NextEntityTurn();
};
