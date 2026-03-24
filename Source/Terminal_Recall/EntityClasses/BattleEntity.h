#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "Terminal_Recall/StructsAndEnums/ActionTypes.h"
#include "Terminal_Recall/StructsAndEnums/GridRangeExport.h"
#include "BattleEntity.generated.h"

class ABattleGridCell;

USTRUCT()
struct FMoveStruct
{
	GENERATED_BODY()
	UPROPERTY()
	FGridRangeExport ActionRange;
	UPROPERTY()
	int32 Cooldown = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChangeCell);

class UActionButtonUI;
class APlayerBattleController;
class ABattleManager;

UCLASS()
class TERMINAL_RECALL_API ABattleEntity : public ABaseEntity
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "For Designers | Necessities")
	TObjectPtr<UDataTable> ActionDataTable;
	UPROPERTY(EditAnywhere, Category = "For Designers | Necessities")
	TSubclassOf<UUserWidget> ActionButtonClass;
	UPROPERTY(BlueprintReadOnly, Category = "For Designers | Delegates")
	FChangeCell OnChangeCell;
protected:
	UPROPERTY()
	TObjectPtr<ABattleManager> BattleManager;
	UPROPERTY(EditAnywhere, Category="For Designers | Entity Info | General Stuff")
	float DamageMultiplier = 1.0f;
	UPROPERTY()
	FIntPoint GridPos = FIntPoint(0.0f, 0.0f);
	UPROPERTY()
	TObjectPtr<APlayerBattleController> PlayerBattleController;
	UPROPERTY()
	TArray<UActionButtonUI*> MovementActionsUI;
	UPROPERTY()
	TArray<UActionButtonUI*> AttackActionsUI;
	UPROPERTY()
	TArray<UActionButtonUI*> ItemActionsUI;
	TArray<FMoveStruct> AttackInfoList; 
	TArray<FMoveStruct> MoveInfoList;
	UPROPERTY()
	TObjectPtr<ABattleGridCell> CurrentCell;
public:
	ABattleEntity();
	virtual void OnBattleStart(ABattleManager* battleManager);
	virtual void OnTurnGo();
	void ShowChoices();
	void CustomTakeDamage(float damage);
	void SetPos(FIntPoint newPos, ABattleGridCell* cell);
	FIntPoint GetPos() { return GridPos; }
	
	FGridRangeExport* GetActionInfo(EActionType type, int32 indexInList);
	virtual void BeginPlay() override;
	virtual void OnMove() override;
	virtual void OnEntityClicked(AActor* TouchedActor, FKey ButtonPressed) override;
protected:
	virtual void FindAndSaveActions(const TArray<int32>& attackArray, const TArray<int32>& moveArray, bool isEnemy = true);
	TOptional<FGridRangeExport> LoadActionInfoFromPath(FString path);
};