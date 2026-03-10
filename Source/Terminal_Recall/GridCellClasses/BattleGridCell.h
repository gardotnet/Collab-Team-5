// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGridCell.h"
#include "Terminal_Recall/StructsAndEnums/ActionType.h"
#include "Terminal_Recall/StructsAndEnums/BattleCellState.h"
#include "BattleGridCell.generated.h"

class ABattleEntity;
class ABaseTrap;

UCLASS()
class TERMINAL_RECALL_API ABattleGridCell : public ABaseGridCell
{
	GENERATED_BODY()

public:
	// add cell colour here
	
protected:
	EBattleCellState CurrentState;
	// [NOTE] current colour
	UPROPERTY()
	TObjectPtr<ABaseTrap> TrapRef;
	bool Walkable = true;
	FVector2f GridPos;
	UPROPERTY()
	TObjectPtr<ABattleEntity> EntityCauserRef;
	
public:
	ABattleGridCell();
	void AssignTrap(TObjectPtr<ABaseTrap> trap);
	void AssignWall();
	void AssignGridPos(FVector2f pos);
	void EnableCell(EBattleCellState isAttack);
	void AddActionCauser(TObjectPtr<ABattleEntity> entity);
protected:
	virtual void OnCellClicked(AActor* TouchedActor, FKey ButtonPressed) override;
};
