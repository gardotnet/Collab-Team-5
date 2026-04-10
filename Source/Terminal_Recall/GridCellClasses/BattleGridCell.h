#pragma once

#include <Terminal_Recall/StructsAndEnums/ActionTypes.h>
#include "CoreMinimal.h"
#include "BaseGridCell.h"
#include "BattleGridCell.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnClickedCell, ABattleEntity*, target, EActionType, type, bool,
                                               mainAttackCell);
class ABaseInteractable;
class ABattleManager;
class APlayerBattleController;
class ABattleEntity;
class UPrimitiveComponent;

UCLASS()
class TERMINAL_RECALL_API ABattleGridCell : public ABaseGridCell
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category="For Designers | Delegates")
	FOnClickedCell OnClickedCellEvent;
	int32 CellGroupID = -1;
	UPROPERTY()
	TObjectPtr<ABaseInteractable> InteractableRef;
protected:
	UPROPERTY()
	TObjectPtr<APlayerBattleController> PlayerController;
	FIntPoint GridPos;
	bool Walkable = true;
	UPROPERTY()
	TObjectPtr<ABattleEntity> EntityCauserRef;
	UPROPERTY()
	TObjectPtr<ABattleEntity> ResidingEntity;
	EActionType ActionType;
	UPROPERTY()
	ABattleManager* BattleManager;
	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	UMaterialInterface* MoveHighlightMat;
	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	UMaterialInterface* HoverHighlightMat;
	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	UMaterialInterface* HitHighlightMat;
	
	UPROPERTY()
	ABaseInteractable* Interactable;
	bool bIsPersistentInteractable;
	
public:
	ABattleGridCell();
	void AssignInteractable(TObjectPtr<ABaseInteractable> interactable);
	bool EnableCell(EActionType actionType, ABattleEntity* causer, int32 groupID = -1);
	UFUNCTION()
	void DisableCell();
	
	UFUNCTION()
	void ForceAttack(int32 groupID);
	UFUNCTION()
	void ForceHover(int32 groupID);
	UFUNCTION()
	void ForceEndHover(int32 groupID);
	
	void HouseEntity(ABattleEntity* entity);
	UFUNCTION()
	void EvictEntity();
	ABattleEntity* GetEntity() { return ResidingEntity; }

	void CellSetUp(FIntPoint gridPos, ABattleManager* battleManager);
	
	FIntPoint GetGridPos() { return GridPos; }
	void SetWalkable(bool walkable);
	bool CanMoveOnTo();
	
	void ForceHighlight(ABattleEntity* causer);
	void EndForceHighlight();
	
	void PassInteractableRef(ABaseInteractable* interactable, bool bIsPersistent);
	
protected:
	virtual void OnCellClicked(AActor* TouchedActor, FKey ButtonPressed) override;
	virtual void OnUnitArrive_Implementation() override;
	UFUNCTION()
	void OnHoverOver(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void OnHoverEnd(UPrimitiveComponent* TouchedComponent);
	
};
