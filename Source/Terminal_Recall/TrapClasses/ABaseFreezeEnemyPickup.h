#pragma once

#include "CoreMinimal.h"
#include "BaseInteractableObj.h"
#include "ABaseFreezeEnemyPickup.generated.h"

UCLASS()
class TERMINAL_RECALL_API ABaseFreezeEnemyPickup : public ABaseInteractableObj
{
	GENERATED_BODY()
	
public:
	ABaseFreezeEnemyPickup();

	virtual void OnUnitArrive_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Freeze Pickup")
	void FreezeEnemies();
};
