#include "BattleGridCell.h"

#include "Terminal_Recall/EntityClasses/BattleEntity.h"

ABattleGridCell::ABattleGridCell()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABattleGridCell::AssignTrap(TObjectPtr<ABaseTrap> trap)
{
	TrapRef = trap;
}

void ABattleGridCell::AssignWall()
{
	Walkable = false;
}

void ABattleGridCell::AssignGridPos(FVector2f pos)
{
	Clickable = false;
	GridPos = pos;
}

void ABattleGridCell::EnableCell(EBattleCellState isAttack)
{
	CurrentState = isAttack;
	Clickable = true;
}

void ABattleGridCell::AddActionCauser(TObjectPtr<ABattleEntity> entity)
{
	EntityCauserRef = entity;
}

void ABattleGridCell::OnCellClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	if (!Clickable)
		return;
	Clickable = false;
	switch (CurrentState)
	{
		case EBattleCellState::Move:
			EntityCauserRef->OnMove(GetActorLocation());
		break;
		case EBattleCellState::Attack:
			EntityCauserRef->DoAttackAction(UnitRef, GetActorLocation());
		break;
		default:
		break;
	}
	CurrentState = EBattleCellState::None;
}




