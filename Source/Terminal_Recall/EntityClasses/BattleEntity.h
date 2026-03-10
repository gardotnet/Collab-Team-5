#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "BattleEntity.generated.h"
class ABattleManager;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTurnDone);
UCLASS()
class TERMINAL_RECALL_API ABattleEntity : public ABaseEntity
{
	GENERATED_BODY()

public:
	
	
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
	
public:
	ABattleEntity();
	virtual void OnBattleStart();
	virtual void OnTurnGo();
	void OnActionSelected();
	void DoAttackAction(TObjectPtr<ABaseEntity> enemy, FVector pos);
	void ShowChoices();
	void HideChoices();

	virtual void BeginPlay() override;
	virtual void OnMove(FVector pos) override;
	virtual void OnEntityClicked(AActor* TouchedActor, FKey ButtonPressed) override;
};


