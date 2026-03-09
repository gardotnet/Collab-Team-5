#pragma once

#include "CoreMinimal.h"

#include "BaseTrap.generated.h"

UCLASS()
class TERMINAL_RECALL_API ABaseTrap : public AActor
{
	GENERATED_BODY()

public:
	ABaseTrap();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(EditAnywhere, Category="Trap Info (For Designers)")
	float Damage;
	UPROPERTY(EditAnywhere, Category="Trap Info (For Designers)", meta=(ToolTip="Enable if you want trap to wait a bit before activating"))
	bool IsDelayed;
	UPROPERTY(EditAnywhere, Category="Trap Info (For Designers)")
	float DelayTime;
	UPROPERTY(EditAnywhere, Category="Trap Info (For Designers)", meta=(ToolTip="The time before trap disable itself"))
	float TrapDisableTime;
	
	UPROPERTY(EditAnywhere, Category="Trap Info (For Designers)", meta=(ToolTip="Enable if you want the trap to always be on"))
	bool IsTrapOn;

	UFUNCTION(BlueprintCallable, Category="Trap Function (For Designers)", meta = (ToolTip = "Will activate the trap if it not activated yet"))
	bool ActivateTrap();
	UFUNCTION(BlueprintCallable, Category="Trap Function (For Designers)", meta = (ToolTip = "Will play trap animation"))
	void StartTrapAnimation();
	UFUNCTION(BlueprintCallable, Category="Trap Function (For Designers)", meta = (ToolTip = "Will disable the trap"))
	void DisableTrap();
};
