#pragma once

#include "CoreMinimal.h"
#include "BaseInteractableObj.h"
#include "ABaseCrackedTiles.generated.h"

UCLASS()
class TERMINAL_RECALL_API ABaseCrackedTiles : public ABaseInteractableObj
{
	GENERATED_BODY()
	
public:
	ABaseCrackedTiles();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cracked Tile")
	int32 MaxStepCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Cracked Tile")
	int32 CurrentStepCount;

	virtual void OnUnitArrive_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cracked Tile")
	void BreakTile();
};
