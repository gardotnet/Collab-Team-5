#pragma once

#include "CoreMinimal.h"
#include "Terminal_Recall/StructsAndEnums/ActionTypes.h"
#include "Terminal_Recall/StructsAndEnums/For2dArray/FGridRowCell.h"
#include "BattleManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCellSelected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNewTurn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndOfPlayerTurn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSetUpDone);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGroupCellDelegate, int32, groupID);

class ABaseInteractable;
class APlayerBattleController;
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

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Stuff For Designers | Delegates")
	FNewTurn OnNewTurnEvent;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Stuff For Designers | Delegates")
	FEndOfPlayerTurn OnPlayerEndTurnEvent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Stuff For Designers | Variables")
	bool bSetUpComplete = false;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Stuff For Designers | Delegates")
	FSetUpDone OnSetUpDone;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stuff For Designers | Necessities")
	FName MainGameLevel = "Viet_Test_Place";
	UPROPERTY(BlueprintReadOnly, Category="Stuff For Designers | Variables")
	TObjectPtr<ABattlePlayerEntity> PlayerRef;
	UPROPERTY(BlueprintReadOnly, Category="Stuff For Designers | Variables")
	TArray<TObjectPtr<ABattleEnemyEntity>> EnemyArray;
	UPROPERTY(BlueprintReadOnly, Category="Stuff For Designers | Variables")
	TMap<FIntPoint, TObjectPtr<ABattleGridCell>> GridMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stuff For Designers | Variables")
	EActionType PlayerActionType = EActionType::Movement;
	
	UPROPERTY()
	TSubclassOf<ABaseInteractable> RobotPickUpClass;
	FIntPoint PickUpLoc = FIntPoint(0, 0); 
	
	UPROPERTY()
	TArray<AActor*> TrapList;
	
	
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
	// allows player to move after battle ended
	bool bFreeMode = false;
	
public:
	ABattleManager();
	void OnEntitySelected(ABattleEntity* entity);
	UFUNCTION()
	void ActionChoiceSelected(UActionButtonUI* selectedButton);
	
	UFUNCTION(BlueprintCallable, Category = "Stuff For Designers | Functions")
	void ActionSelected(int hotbarSlot);
	
	UFUNCTION(BlueprintCallable, Category = "Stuff For Designers | Functions")
	void ToggleMoveOrAttack();
	
	UFUNCTION()
	void OnCellSelected(ABattleEntity* target, EActionType type, bool mainAttackCell);
	UFUNCTION(BlueprintCallable, Category = "Stuff For Designers | Function")
	void NextEntityTurn();
	
	UFUNCTION(BlueprintCallable, Category = "Stuff For Designers | Function")
	inline void ClearPrediction();

	UFUNCTION()
	void EndCombat();
protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
};
