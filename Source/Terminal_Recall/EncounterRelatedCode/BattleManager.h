#pragma once

#include "CoreMinimal.h"
#include "Terminal_Recall/StructsAndEnums/For2dArray/FGridRowCell.h"
#include "BattleManager.generated.h"

class APlayerBattleController;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCellSelected);

class UActionButtonUI;
class ABattleEntity;
class ABattleEnemyEntity;
class ABattlePlayerEntity;

UCLASS()
class TERMINAL_RECALL_API ABattleManager : public AActor
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	TObjectPtr<ABattlePlayerEntity> PlayerRef;
	TArray<TObjectPtr<ABattleEnemyEntity>> EnemyArray;
	UPROPERTY()
	TMap<FIntPoint, TObjectPtr<ABattleGridCell>> GridMap;
	
protected:
	int32 EntityIndex = -1;
	UPROPERTY()
	TObjectPtr<ABattleEntity> CurrentEntityChosen;
	UPROPERTY()
	TObjectPtr<APlayerBattleController> PlayerBattleController;

	FCellSelected CellSelectedEvent;
	
	bool SelectMode = false;

public:
	ABattleManager();
	void SetUpBattle();
	void OnEntitySelected(ABattleEntity* entity);
	UFUNCTION()
	void ActionChoiceSelected(UActionButtonUI* selectedButton);
	
	UFUNCTION()
	void OnCellSelected(ABattleEntity* target);
protected:
	UFUNCTION()
	void NextEntityTurn();

	virtual void BeginPlay() override;
};
