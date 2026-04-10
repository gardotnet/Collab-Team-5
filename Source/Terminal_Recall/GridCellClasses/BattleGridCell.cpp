#include "BattleGridCell.h"

#include "Terminal_Recall/EncounterRelatedCode/BattleManager.h"
#include "Terminal_Recall/EntityClasses/BattleEntity.h"
#include "Terminal_Recall/TrapClasses/BaseTrapLatest.h"
#include "Terminal_Recall/InteractableObject/BaseInteractable.h"

ABattleGridCell::ABattleGridCell()
{
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent->SetOverlayMaterialMaxDrawDistance(5000);
	MeshComponent->OnBeginCursorOver.AddDynamic(this, &ABattleGridCell::OnHoverOver);
	MeshComponent->OnEndCursorOver.AddDynamic(this, &ABattleGridCell::OnHoverEnd);
}
void ABattleGridCell::AssignInteractable(TObjectPtr<ABaseInteractable> interactable)
{
	InteractableRef = interactable;
	interactable->ParentGridCell = this;
	InteractableRef->OnLinked();
}
bool ABattleGridCell::EnableCell(EActionType actionType, ABattleEntity* causer, int32 groupID)
{
	if (IsValid(ResidingEntity) && (actionType != EActionType::Attack && actionType != EActionType::EnemyAttack))
		return false;
	if (!Walkable || Clickable)
		return false;
	
	EntityCauserRef = causer;
	ActionType = actionType;
	CellGroupID = groupID;
	MeshComponent->SetCollisionProfileName("BlockAll");
	Clickable = true;
	MeshComponent->SetOverlayMaterial(MoveHighlightMat);
	if (IsValid(ResidingEntity) && (actionType == EActionType::Attack || actionType == EActionType::EnemyAttack))
		ResidingEntity->ShowPotentialHit(EntityCauserRef->GetDamage());
	return true;
}
void ABattleGridCell::DisableCell()
{
	MeshComponent->SetCollisionProfileName("NoCollision");
	MeshComponent->SetOverlayMaterial(nullptr);
	if (IsValid(ResidingEntity))
		ResidingEntity->HidePotentialHit();
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
	{
		if (IsValid(ResidingEntity))
			MeshComponent->SetOverlayMaterial(HitHighlightMat);
		else
			MeshComponent->SetOverlayMaterial(HoverHighlightMat);
	}
}
void ABattleGridCell::ForceEndHover(int32 groupID)
{
	if (groupID == CellGroupID)
		MeshComponent->SetOverlayMaterial(MoveHighlightMat);
}
void ABattleGridCell::HouseEntity(ABattleEntity* entity)
{
	ResidingEntity = entity;
	if (InteractableRef != nullptr)
		InteractableRef->OnActivate_Implementation(entity);
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

void ABattleGridCell::ForceHighlight(ABattleEntity* causer)
{
	if (IsValid(ResidingEntity))
	{
		ResidingEntity->ShowPotentialHit(causer->GetDamage());
		MeshComponent->SetOverlayMaterial(HitHighlightMat);
		return;
	}
	MeshComponent->SetOverlayMaterial(MoveHighlightMat);
}

void ABattleGridCell::EndForceHighlight()
{
	MeshComponent->SetOverlayMaterial(nullptr);
	if (IsValid(ResidingEntity))
		ResidingEntity->HidePotentialHit();
}

void ABattleGridCell::PassInteractableRef(ABaseInteractable* interactable, bool bIsPersistent)
{
	Interactable = interactable;
	bIsPersistentInteractable = bIsPersistent;
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

void ABattleGridCell::OnUnitArrive_Implementation()
{
	Super::OnUnitArrive_Implementation();
	OnUnitArrive();
	if (IsValid(Interactable))
		Interactable->OnActivate();
}

void ABattleGridCell::OnHoverOver(UPrimitiveComponent* TouchedComponent)
{
	if (Clickable)
	{
		if (IsValid(ResidingEntity))
			MeshComponent->SetOverlayMaterial(HitHighlightMat);
		else
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