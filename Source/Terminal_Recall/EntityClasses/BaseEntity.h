#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveDone);

UCLASS()
class TERMINAL_RECALL_API ABaseEntity : public APawn
{
	GENERATED_BODY()

public:
	ABaseEntity();
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMoveDone OnMoveDoneEvent; // [NOTE] Make sure to use this event when player reached destination
	
	UPROPERTY(EditInstanceOnly, Category="Entity Info (For Designers)")
	float MaxHealth = 100;
	UPROPERTY(EditInstanceOnly, Category="Entity Info (For Designers)")
	int MoveType = 0; // [NOTE] will be changed for an enum
	UPROPERTY(EditInstanceOnly, Category="Entity Info (For Designers)", meta = (ToolTip = "How many cells entity can move in a certain direction"))
	int MoveMagnitude;
	
	float CurrentHealth;
	
	virtual void BeginPlay() override;
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void OnMove(FVector3f pos);
};
