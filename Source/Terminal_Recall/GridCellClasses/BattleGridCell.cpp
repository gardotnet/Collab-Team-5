#include "BattleGridCell.h"

#include "Terminal_Recall/EncounterRelatedCode/BattleManager.h"
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
	//TrapRef = trap;
}
bool ABattleGridCell::EnableCell(EActionType actionType, ABattleEntity* causer, int32 groupID)
{
	if (ResidingEntity && !(actionType == EActionType::Attack || actionType == EActionType::EnemyAttack))
		return false;
	if (!Walkable || Clickable)
		return false;
	EntityCauserRef = causer;
	ActionType = actionType;
	CellGroupID = groupID;
	MeshComponent->SetCollisionProfileName("BlockAll");
	Clickable = true;
	MeshComponent->SetOverlayMaterial(MoveHighlightMat);
	return true;
}
void ABattleGridCell::DisableCell()
{
	MeshComponent->SetCollisionProfileName("NoCollision");
	MeshComponent->SetOverlayMaterial(nullptr);
	Clickable = false;
}
void ABattleGridCell::ForceAttack(int32 groupID)
{
	if (groupID == CellGroupID)
		OnClickedCellEvent.Broadcast(ResidingEntity, ActionType, false);
}
void ABattleGridCell::ForceHover(int32 groupID)
{
	if (groupID == CellGroupID)
		MeshComponent->SetOverlayMaterial(HoverHighlightMat);
}
void ABattleGridCell::ForceEndHover(int32 groupID)
{
	if (groupID == CellGroupID)
		MeshComponent->SetOverlayMaterial(MoveHighlightMat);
}
void ABattleGridCell::HouseEntity(ABattleEntity* entity)
{
	ResidingEntity = entity;
	OnUnitArrive();
	entity->OnChangeCell.AddDynamic(this, &ABattleGridCell::EvictEntity);
}
void ABattleGridCell::EvictEntity()
{
	ResidingEntity->OnChangeCell.RemoveDynamic(this, &ABattleGridCell::EvictEntity);
	ResidingEntity = nullptr;
}
void ABattleGridCell::CellSetUp(FIntPoint gridPos, ABattleManager* battleManager)
{
	GridPos = gridPos;
	BattleManager = battleManager;
}
void ABattleGridCell::SetWalkable(bool walkable)
{
	Walkable = walkable;
}

bool ABattleGridCell::CanMoveOnTo()
{
	if (IsValid(ResidingEntity) || !Walkable)
		return false;
	return true;
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
			BattleManager->CallGroupAttackEvent.Broadcast(CellGroupID);
			OnClickedCellEvent.Broadcast(ResidingEntity, ActionType, true);
			
		break;
		case EActionType::Movement:
			EntityCauserRef->SetPos(GridPos, this);
			HouseEntity(EntityCauserRef);
			EntityCauserRef->StartMove(GetActorLocation());
			OnClickedCellEvent.Broadcast(nullptr, ActionType, false);
		break;
		case EActionType::EnemyAttack:
			OnClickedCellEvent.Broadcast(nullptr, ActionType, true);
		case EActionType::EnemyMove:
			OnClickedCellEvent.Broadcast(nullptr, ActionType, false);
		default:
		break;
	}
}
void ABattleGridCell::OnHoverOver(UPrimitiveComponent* TouchedComponent)
{
	if (Clickable)
	{
		MeshComponent->SetOverlayMaterial(HoverHighlightMat);
		if (ActionType == EActionType::Attack)
			BattleManager->CallGroupHoverEvent.Broadcast(CellGroupID);
	}
}
void ABattleGridCell::OnHoverEnd(UPrimitiveComponent* TouchedComponent)
{
	if (Clickable)
	{
		MeshComponent->SetOverlayMaterial(MoveHighlightMat);
		if (ActionType == EActionType::Attack)
			BattleManager->CallGroupHoverEndEvent.Broadcast(CellGroupID);
	}
}