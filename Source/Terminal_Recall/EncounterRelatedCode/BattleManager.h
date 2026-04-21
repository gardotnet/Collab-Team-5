#pragma once

#include "CoreMinimal.h"
#include "BattleLevelLoader.h"
#include "Terminal_Recall/StructsAndEnums/ActionTypes.h"
#include "Terminal_Recall/StructsAndEnums/For2dArray/FGridRowCell.h"
#include "BattleManager.generated.h"

class APlayerBattleController;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCellSelected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGroupCellDelegate, int32, groupID);

class UActionButtonUI;
class ABattleEntity;
class ABattleEnemyEntity;
class ABattlePlayerEntity;

UCLASS()
class TERMINAL_RECALL_API ABattleManager : public AActor
{
	GENERATED_BODY()

public:
	FGroupCellDelegate CallGroupHoverEvent;
	FGroupCellDelegate CallGroupHoverEndEvent;
	FGroupCellDelegate CallGroupAttackEvent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stuff For Designers | Necessities")
	FName MainGameLevel = "Viet_Test_Place";
	UPROPERTY(BlueprintReadOnly, Category="Stuff For Designers | Variables")
	TObjectPtr<ABattlePlayerEntity> PlayerRef;
	UPROPERTY(BlueprintReadOnly, Category="Stuff For Designers | Variables")
	TArray<TObjectPtr<ABattleEnemyEntity>> EnemyArray;
	UPROPERTY(BlueprintReadOnly, Category="Stuff For Designers | Variables")
	TMap<FIntPoint, TObjectPtr<ABattleGridCell>> GridMap;
	FIntPoint GridSize;

protected:
	int32 SavedActionId = -1;
	int32 EntityIndex = -1;
	UPROPERTY(BlueprintReadOnly, Category="Stuff For Designers | Variables")
	TObjectPtr<ABattleEntity> CurrentEntityChosen;
	UPROPERTY()
	TObjectPtr<APlayerBattleController> PlayerBattleController;
	
	FCellSelected CellSelectedEvent;
	// to disable player clicking entity choices while clicking grid
	bool SelectMode = false;
public:
	ABattleManager();
	void OnEntitySelected(ABattleEntity* entity);
	UFUNCTION()
	void ActionChoiceSelected(UActionButtonUI* selectedButton);
	UFUNCTION()
	void OnCellSelected(ABattleEntity* target, EActionType type, bool mainAttackCell);
	UFUNCTION(BlueprintCallable, Category = "Stuff For Designers | Function")
	void NextEntityTurn();
	UFUNCTION()
	void findGridSize();
protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
};
