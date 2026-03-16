#include "BattleGridCell.h"


ABattleGridCell::ABattleGridCell()
{
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent->SetOverlayMaterialMaxDrawDistance(5000);
	MeshComponent->OnBeginCursorOver.AddDynamic(this, &ABattleGridCell::OnHoverOver);
	MeshComponent->OnEndCursorOver.AddDynamic(this, &ABattleGridCell::OnHoverEnd);
}
void ABattleGridCell::AssignTrap(TObjectPtr<ABaseTrap> trap)
{
	TrapRef = trap;
}
void ABattleGridCell::EnableCell()
{
	if (!Walkable)
		return;
	Clickable = true;
	MeshComponent->SetOverlayMaterial(MoveHighlightMat);
}
void ABattleGridCell::DisableCell()
{
	OnClickedCellEvent.Clear();
	MeshComponent->SetOverlayMaterial(nullptr);
	Clickable = false;
}
void ABattleGridCell::SetGridPos(FIntPoint gridPos)
{
	GridPos = gridPos;
}
void ABattleGridCell::SetWalkable(bool walkable)
{
	Walkable = walkable;
}
void ABattleGridCell::OnCellClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	if (!Clickable)
		return;
	Clickable = false;
	OnClickedCellEvent.Broadcast(this);
	/*switch (CurrentState)
	{
		case EBattleCellState::Move:
			EntityCauserRef->StartMove(GetActorLocation());
		break;
		case EBattleCellState::Attack:
			EntityCauserRef->DoAttackAction(UnitRef, GetActorLocation());
		break;
		default:
		break;
	}
	CurrentState = EBattleCellState::None;*/
}

void ABattleGridCell::OnHoverOver(UPrimitiveComponent* TouchedComponent)
{
	UE_LOG(LogTemp, Display, TEXT("OnHoverOver"));
	if (Clickable)
		MeshComponent->SetOverlayMaterial(HoverHighlightMat);
}

void ABattleGridCell::OnHoverEnd(UPrimitiveComponent* TouchedComponent)
{
	UE_LOG(LogTemp, Display, TEXT("OnHoverEnd"));
	if (Clickable)
		MeshComponent->SetOverlayMaterial(MoveHighlightMat);
}






