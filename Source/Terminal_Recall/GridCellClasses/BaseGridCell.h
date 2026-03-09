#pragma once

#include "CoreMinimal.h"
#include "Terminal_Recall/EntityClasses/BaseEntity.h"
#include "Terminal_Recall/TrapClasses/BaseTrap.h"
#include "BaseGridCell.generated.h"

class UStaticMeshComponent;

UCLASS()
class TERMINAL_RECALL_API ABaseGridCell : public AActor
{
	GENERATED_BODY()

public:
	ABaseGridCell();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	/*UPROPERTY(EditAnywhere, Category="Grid Cell Info (For Designers)", meta=(ToolTip="Disable to prevent player being able to step on this cell"))
	bool Walkable = true;
	UPROPERTY(EditAnywhere, Category="Grid Cell Info (For Designers)", meta=(ToolTip="Enable this if this grid cell has a trap and assign it below"))
	bool HasTrap = false;
	UPROPERTY(EditAnywhere, Category="Grid Cell Info (For Designers)", meta=(ToolTip="Select the trap object related to this cell here", EditCondition = "HasTrap"))
	TObjectPtr<ABaseTrap> TrapRef;*/

	virtual void BeginPlay() override;
	virtual void OnUnitArrive();
	
private:
	UPROPERTY()
	TObjectPtr<ABaseEntity> UnitRef;
	UFUNCTION()
	virtual void OnCellClicked(AActor* TouchedActor, FKey ButtonPressed);
};
