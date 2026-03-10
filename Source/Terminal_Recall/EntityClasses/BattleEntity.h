#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "BattleEntity.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTurnDone);
UCLASS()
class TERMINAL_RECALL_API ABattleEntity : public ABaseEntity
{
	GENERATED_BODY()

public:
	ABattleEntity();
	
	UPROPERTY(BlueprintAssignable, Category = "Events For Designers")
	FOnTurnDone OnTurnDoneEvent; // [NOTE] make sure to call this event when actions (e.g. move, attack) are done

	UPROPERTY(EditAnywhere, Category="Entity Info (For Designers)")
	float DamageMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category="Entity Info (For Designers)")
	TArray<int32> AttackListID;
	
	//UPROPERTY(EditAnywhere, Category="Entity Info (For Designers)", meta = (EditCondition = "false")) 
	// [NOTE] store movement option button
	// [NOTE] add list to store attack option button

	virtual void OnBattleStart();
	virtual void OnTurnGo();
	void OnMovementSelected();
	void OnAttackSelected(int index);
	void OnAttack(int id, FVector3f pos);
	void ShowChoices();
	void HideChoices();

	virtual void OnMove(FVector pos) override;
	virtual void OnEntityClicked(AActor* TouchedActor, FKey ButtonPressed) override;
};


