#include "BattleGridCell.h"
#include "Terminal_Recall/EntityClasses/BattleEntity.h"


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

bool ABattleGridCell::EnableCell(EActionType actionType,  ABattleEntity* causer)
{
	if (ResidingEntity && actionType != EActionType::Attack)
		return false;
	if (!Walkable || Clickable)
		return false;
	EntityCauserRef = causer;
	ActionType = actionType;
	MeshComponent->SetCollisionProfileName("BlockAll");
	Clickable = true;
	MeshComponent->SetOverlayMaterial(MoveHighlightMat);
	return true;
}

void ABattleGridCell::DisableCell()
{
	OnClickedCellEvent.Clear();
	MeshComponent->SetCollisionProfileName("NoCollision");
	MeshComponent->SetOverlayMaterial(nullptr);
	Clickable = false;
}

void ABattleGridCell::HouseEntity(ABattleEntity* entity)
{
	ResidingEntity = entity;
	entity->OnChangeCell.AddDynamic(this, &ABattleGridCell::EvictEntity);
}

void ABattleGridCell::EvictEntity()
{
	ResidingEntity = nullptr;
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
	switch (ActionType)
	{
		case EActionType::Attack:
			// Tells listeners "I Need You To Find This Guy And #### Him Up!" 
			OnClickedCellEvent.Broadcast(ResidingEntity);
		break;
		case EActionType::Movement:
			EntityCauserRef->SetPos(GridPos);
			EntityCauserRef->StartMove(GetActorLocation());
			OnClickedCellEvent.Broadcast(nullptr);
		break;
		default:
		break;
	}
}

void ABattleGridCell::OnHoverOver(UPrimitiveComponent* TouchedComponent)
{
	if (Clickable)
		MeshComponent->SetOverlayMaterial(HoverHighlightMat);
}

void ABattleGridCell::OnHoverEnd(UPrimitiveComponent* TouchedComponent)
{
	if (Clickable)
		MeshComponent->SetOverlayMaterial(MoveHighlightMat);
}