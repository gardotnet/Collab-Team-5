#pragma once

#include "CoreMinimal.h"
#include "BaseClassInteractables/BaseInteractableObj.h"
#include "ABaseExplosiveTile.generated.h"

UCLASS()
class TERMINAL_RECALL_API ABaseExplosiveTile : public ABaseInteractableObj
{
	GENERATED_BODY()
	
public:
	ABaseExplosiveTile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosive")
	float ExplosionDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosive")
	float ExplosionRadius;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Explosive")
	void Explode();
};
