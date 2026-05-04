#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "Terminal_Recall/StructsAndEnums/ActionTypes.h"
#include "Terminal_Recall/StructsAndEnums/EntityTwinActionContainer.h"
#include "Terminal_Recall/StructsAndEnums/GridRangeExport.h"
#include "BattleEntity.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChangeCell);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FForCooldown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamaged, float, newHp);

class UTextRenderComponent;
class ABattleGridCell;
class UActionButtonUI;
class APlayerBattleController;
class ABattleManager;
class ABaseStatusEffect;

UCLASS()
class TERMINAL_RECALL_API ABattleEntity : public ABaseEntity
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Stuff For Designers | Necessities")
	TObjectPtr<UDataTable> ActionDataTable;
	UPROPERTY(BlueprintAssignable, Category = "Stuff For Designers | Delegates")
	FChangeCell OnChangeCell;
	int32 SelectedActionID = -1;

	bool bRestrictedMovement = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stuff For Designers | Variables")
	TArray<ABaseStatusEffect*> CurrentStatusEffects;
	
	UPROPERTY(BlueprintAssignable, Category = "Stuff For Designers | Delegates")
	FOnDamaged OnDamagedEvent;
	
protected:
	UPROPERTY()
	TObjectPtr<UTextRenderComponent> TextRenderComponent;
	UPROPERTY()
	TObjectPtr<ABattleManager> BattleManager;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stuff For Designers | Entity Info | General Stuff")
	float DamageMultiplier = 1.0f;
	UPROPERTY()
	FIntPoint GridPos = FIntPoint(0.0f, 0.0f);
	UPROPERTY()
	TObjectPtr<APlayerBattleController> PlayerBattleController;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Stuff For Designers | Variables")
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
	UFUNCTION(BlueprintCallable, Category = "Stuff For Designers | Functions")
	virtual void CustomTakeDamageAndApplyStatus(float damage, ABaseStatusEffect* effect);
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
	
	inline void HandleStatusEffects();
};