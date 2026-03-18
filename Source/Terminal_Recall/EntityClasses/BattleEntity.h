#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "Terminal_Recall/StructsAndEnums/ActionTypes.h"
#include "Terminal_Recall/StructsAndEnums/GridRangeExport.h"
#include "Terminal_Recall/StructsAndEnums/ForDataTable/ActionStruct.h"
#include "BattleEntity.generated.h"

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
	
	FChangeCell OnChangeCell;

protected:
	
	float Health = 1.0f;
	
	UPROPERTY()
	TObjectPtr<ABattleManager> BattleManager;
	
	UPROPERTY(EditAnywhere, Category="Entity Info (For Designers)")
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
	
	TArray<FGridRangeExport> AttackInfoList; 
	TArray<FGridRangeExport> MoveInfoList; 
	
public:
	ABattleEntity();
	virtual void OnBattleStart(ABattleManager* battleManager, float health, float damageMultiplier);
	virtual void OnTurnGo();
	void ShowChoices();
	void CustomTakeDamage(float damage);
	void SetPos(FIntPoint newPos);
	
	FIntPoint GetPos() { return GridPos; }
	FGridRangeExport* GetActionInfo(EActionType type, int32 indexInList);
	

	virtual void BeginPlay() override;
	virtual void OnMove() override;
	virtual void OnEntityClicked(AActor* TouchedActor, FKey ButtonPressed) override;
	
	
protected:
	
	TOptional<FGridRangeExport> LoadActionInfoFromPath(FString path);
	
};


