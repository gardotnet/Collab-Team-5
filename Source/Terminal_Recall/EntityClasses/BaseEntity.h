#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.generated.h"

class AOurBasePlayerController;
class ADefaultPlayerController;
class UCapsuleComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveDone);

UCLASS()
class TERMINAL_RECALL_API ABaseEntity : public APawn
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(BlueprintAssignable, Category = "Events For Designers || Delegates")
	FOnMoveDone OnMoveDoneEvent; // [NOTE] Make sure to use this event when player reached destination
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stuff For Designers | Entity Info | General Stuff")
	float MaxHealth = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stuff For Designers | Entity Info | General Stuff", meta=(EditCondition = "false"))
	float CurrentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stuff For Designers | Entity Info | General Stuff")
	float Speed = 200;
	
	float DesiredMoveDuration = 0;
	float MoveTimer;
	FVector TargetPosition;
	UPROPERTY()
	TObjectPtr<AOurBasePlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category="Stuff For Designers | Variables")
	FTimerHandle MoveTimerHandle;
	FTimerManager* TimerManagerRef;

protected:
	ABaseEntity();
	virtual void BeginPlay() override;
	virtual void OnMove();
	
public:
	void StartMove(FVector position);
	UFUNCTION()
	virtual void OnEntityClicked(AActor* TouchedActor, FKey ButtonPressed);
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
