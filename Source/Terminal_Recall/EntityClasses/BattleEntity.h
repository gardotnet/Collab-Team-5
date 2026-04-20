#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "Terminal_Recall/StructsAndEnums/ActionTypes.h"
#include "Terminal_Recall/StructsAndEnums/EntityTwinActionContainer.h"
#include "Terminal_Recall/StructsAndEnums/GridRangeExport.h"
#include "BattleEntity.generated.h"

class UTextRenderComponent;
class ABattleGridCell;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChangeCell);

class UActionButtonUI;
class APlayerBattleController;
class ABattleManager;

UCLASS()
class TERMINAL_RECALL_API ABattleEntity : public ABaseEntity
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Stuff For Designers | Necessities")
	TObjectPtr<UDataTable> ActionDataTable;
	UPROPERTY(EditAnywhere, Category = "Stuff For Designers | Necessities")
	TSubclassOf<UUserWidget> ActionButtonClass;
	UPROPERTY(BlueprintReadOnly, Category = "Stuff For Designers | Delegates")
	FChangeCell OnChangeCell;
	int32 SelectedActionID = -1;

	bool bRestrictedMovement = false;
	
protected:
	UPROPERTY()
	TObjectPtr<UTextRenderComponent> TextRenderComponent;
	UPROPERTY()
	TObjectPtr<ABattleManager> BattleManager;
	UPROPERTY(EditAnywhere, Category="Stuff For Designers | Entity Info | General Stuff")
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
	TArray<FEntityTwinActionContainer> ActionList;
	UPROPERTY()
	TObjectPtr<ABattleGridCell> CurrentCell;
	
public:
	ABattleEntity();
	virtual void OnBattleStart(ABattleManager* battleManager);
	virtual void OnBattleStart(ABattleManager* battleManager, int32 enemyID);
	virtual void OnTurnGo();
	void ShowChoices();
	UFUNCTION(BlueprintCallable, Category = "Stuff For Designers | Functions")
	virtual void CustomTakeDamage(float damage);
	void SetPos(FIntPoint newPos, ABattleGridCell* cell);
	FIntPoint GetPos() { return GridPos; }
	
	virtual void BeginPlay() override;
	virtual void OnMove() override;
	virtual void OnEntityClicked(AActor* TouchedActor, FKey ButtonPressed) override;

	void OnActionComplete(EActionType type);
	void AddCooldown(EActionType type);
	void ReduceCooldown();
	
	UFUNCTION(BlueprintCallable, Category = "Stuff For Designers | Functions")
	void ShowPotentialHit(float damage);
	UFUNCTION(BlueprintCallable, Category = "Stuff For Designers | Functions")
	void HidePotentialHit();
	
	int32 GetDamage() { return ActionList[SelectedActionID].AttackAction.Damage; }
	FGridRangeExport* GetActionInfo(EActionType type, int32 indexInList);

	void EnableRestrictedMovement();
	void DisableRestrictedMovement();
	
protected:
	virtual void FindAndSaveActions(const TArray<int32>& attackArray, const TArray<int32>& moveArray, bool isEnemy = true);
	TOptional<FGridRangeExport> LoadActionInfoFromPath(FString path);
};