// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Terminal_Recall/StructsAndEnums/ActionTypes.h>

#include "CoreMinimal.h"
#include "BaseGridCell.h"
#include "BattleGridCell.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedCell, ABattleEntity*, target);

class APlayerBattleController;
class ABattleEntity;
class ABaseTrap;
class UPrimitiveComponent;

UCLASS()
class TERMINAL_RECALL_API ABattleGridCell : public ABaseGridCell
{
	GENERATED_BODY()

public:
	// add cell colour here
	FOnClickedCell OnClickedCellEvent;

	
protected:
	
	UPROPERTY()
	TObjectPtr<APlayerBattleController> PlayerController;
	
	UPROPERTY()
	TObjectPtr<ABaseTrap> TrapRef;
	FIntPoint GridPos;
	bool Walkable = true;
	UPROPERTY()
	TObjectPtr<ABattleEntity> EntityCauserRef;
	UPROPERTY()
	TObjectPtr<ABattleEntity> ResidingEntity;
	EActionType ActionType;

	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	UMaterialInterface* MoveHighlightMat;
	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	UMaterialInterface* HoverHighlightMat;
	
public:
	ABattleGridCell();
	void AssignTrap(TObjectPtr<ABaseTrap> trap);
	void EnableCell(EActionType actionType, ABattleEntity* causer);
	UFUNCTION()
	void DisableCell();
	
	void HouseEntity(ABattleEntity* entity);
	UFUNCTION()
	void EvictEntity();

	void SetGridPos(FIntPoint gridPos);
	FIntPoint GetGridPos() { return GridPos; };
	void SetWalkable(bool walkable);
	bool GetWalkable() { return Walkable; };
	
protected:
	virtual void OnCellClicked(AActor* TouchedActor, FKey ButtonPressed) override;
	UFUNCTION()
	void OnHoverOver(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void OnHoverEnd(UPrimitiveComponent* TouchedComponent);
};
