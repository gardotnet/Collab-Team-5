#pragma once

#include "CoreMinimal.h"
#include "Terminal_Recall/EntityClasses/BaseEntity.h"
#include "BaseGridCell.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class TERMINAL_RECALL_API ABaseGridCell : public AActor
{
	GENERATED_BODY()

public:
	ABaseGridCell();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BoxComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variables (For Designers)")
	bool Clickable;
	
	/*
	UPROPERTY(EditAnywhere, Category="Grid Cell Info (For Designers)", meta=(ToolTip="Enable this if this grid cell has a trap and assign it below"))
	bool HasTrap = false;
	UPROPERTY(EditAnywhere, Category="Grid Cell Info (For Designers)", meta=(ToolTip="Select the trap object related to this cell here", EditCondition = "HasTrap"))
	TObjectPtr<ABaseTrap> TrapRef;*/
	
	UPROPERTY()
	TObjectPtr<ABaseEntity> UnitRef;

	virtual void BeginPlay() override;
	virtual void OnUnitArrive();
	
private:
	
	UFUNCTION()
	virtual void OnCellClicked(AActor* TouchedActor, FKey ButtonPressed);
};
