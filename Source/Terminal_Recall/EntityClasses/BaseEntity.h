#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.generated.h"

class ADefaultPlayerController;
class UCapsuleComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveDone);

UCLASS()
class TERMINAL_RECALL_API ABaseEntity : public APawn
{
	GENERATED_BODY()

public:
	ABaseEntity();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(BlueprintAssignable, Category = "Events For Designers")
	FOnMoveDone OnMoveDoneEvent; // [NOTE] Make sure to use this event when player reached destination
	
	UPROPERTY(EditAnywhere, Category="Stuff For Designers | Entity Info | General Stuff")
	float MaxHealth = 100;
	UPROPERTY(EditAnywhere, Category="Stuff For Designers | Entity Info | General Stuff")
	float Speed = 200;
	
	float DesiredMoveDuration = 0;
	float MoveTimer;
	FVector TargetPosition;
	TObjectPtr<ADefaultPlayerController> PlayerController;
	
	
	FTimerHandle MoveTimerHandle;
	FTimerManager* TimerManagerRef;
	/*UPROPERTY(EditAnywhere, Category="Entity Info (For Designers)")
	EActionTypeEnum MoveType = EActionTypeEnum::None;
	UPROPERTY(EditAnywhere, Category="Entity Info (For Designers)", meta = (ToolTip = "How many cells entity can move in a certain direction"))
	int MoveMagnitude;*/

	UPROPERTY(EditAnywhere, Category="Entity Info (For Designers)", meta=(EditCondition = "false"))
	float CurrentHealth;
	
	virtual void BeginPlay() override;
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void StartMove(FVector position);
	virtual void OnMove();
	UFUNCTION()
	virtual void OnEntityClicked(AActor* TouchedActor, FKey ButtonPressed);
};
