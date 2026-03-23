#pragma once

#include "CoreMinimal.h"
#include "BaseInteractableObj.h"
#include "ABaseQuicksandTile.generated.h"

UCLASS()
class TERMINAL_RECALL_API ABaseQuicksandTile : public ABaseInteractableObj
{
	GENERATED_BODY()
	
public:
	ABaseQuicksandTile();

	virtual void OnUnitArrive_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quicksand Tile")
	void OnUnitLeave(AActor* LeavingActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quicksand Tile")
	void ApplySlowdown(AActor* TargetActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quicksand Tile")
	void RemoveSlowdown(AActor* TargetActor);
};
