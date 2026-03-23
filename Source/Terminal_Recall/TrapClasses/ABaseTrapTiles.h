#pragma once

#include "CoreMinimal.h"
#include "BaseClassInteractables/BaseInteractableObj.h"
#include "ABaseTrapTiles.generated.h"

UCLASS()
class TERMINAL_RECALL_API ABaseTrapTiles : public ABaseInteractableObj
{
	GENERATED_BODY()
	
public:
	ABaseTrapTiles();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap Tile")
	float DamageAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap Tile")
	int32 TurnsToRestrict;

	virtual void OnUnitArrive_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Trap Tile")
	void ApplyDamageAndRestriction(AActor* TargetActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Trap Tile")
	void ResetTrap();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Trap Tile")
	void ActivateTrapFromAttack();
};
