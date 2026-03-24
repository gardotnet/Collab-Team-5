#pragma once

#include "CoreMinimal.h"
#include "BaseClassInteractables/BaseInteractableObj.h"
#include "ABaseHealthPickup.generated.h"

UCLASS()
class TERMINAL_RECALL_API ABaseHealthPickup : public ABaseInteractableObj
{
	GENERATED_BODY()
	
public:
	ABaseHealthPickup();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Pickup")
	float HealAmount;

	virtual void OnUnitArrive_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Health Pickup")
	void HealPlayer(AActor* PlayerActor);
};
