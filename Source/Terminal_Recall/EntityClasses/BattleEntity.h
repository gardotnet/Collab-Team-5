#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "BattleEntity.generated.h"
class UActionButtonUI;
class APlayerBattleController;
class ABattleManager;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTurnDone);
UCLASS()
class TERMINAL_RECALL_API ABattleEntity : public ABaseEntity
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "For Designers | Necessities")
	TSubclassOf<UUserWidget> ActionButtonClass;
	
	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "Events For Designers")
	FOnTurnDone OnTurnDoneEvent; // [NOTE] make sure to call this event when actions (e.g. move, attack) are done

	UPROPERTY()
	TObjectPtr<ABattleManager> BattleManager;
	
	UPROPERTY(EditAnywhere, Category="Entity Info (For Designers)")
	float DamageMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category="Entity Info (For Designers)")
	TArray<int32> AttackListID;
	
	//UPROPERTY(EditAnywhere, Category="Entity Info (For Designers)", meta = (EditCondition = "false")) 
	// [NOTE] add list to store all option button
protected:
	UPROPERTY()
	FVector2f GridPos = FVector2f(0.0f, 0.0f);
	
	UPROPERTY()
	TObjectPtr<APlayerBattleController> PlayerBattleController;

	UPROPERTY()
	TArray<UActionButtonUI*> MovementActionsUI;
	UPROPERTY()
	TArray<UActionButtonUI*> AttackActionsUI;
	UPROPERTY()
	TArray<UActionButtonUI*> ItemActionsUI;
	
public:
	ABattleEntity();
	virtual void OnBattleStart();
	virtual void OnTurnGo();
	void OnActionSelected();
	void DoAttackAction(TObjectPtr<ABaseEntity> enemy, FVector pos);
	void ShowChoices();
	void HideChoices();

	virtual void BeginPlay() override;
	virtual void OnMove() override;
	virtual void OnEntityClicked(AActor* TouchedActor, FKey ButtonPressed) override;
};


