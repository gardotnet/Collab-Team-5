#pragma once

#include <Terminal_Recall/StructsAndEnums/ActionTypes.h>
#include "CoreMinimal.h"
#include "BaseGridCell.h"
#include "BattleGridCell.generated.h"

class ABattleManager;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnClickedCell, ABattleEntity*, target, EActionType, type, bool, mainAttackCell);

class APlayerBattleController;
class ABattleEntity;
class ABaseTrap;
class UPrimitiveComponent;

UCLASS()
class TERMINAL_RECALL_API ABattleGridCell : public ABaseGridCell
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category="For Designers | Delegates")
	FOnClickedCell OnClickedCellEvent;
	int32 CellGroupID = -1;
protected:
	UPROPERTY()
	TObjectPtr<APlayerBattleController> PlayerController;
	/*UPROPERTY()
	TObjectPtr<ABaseTrap> TrapRef;*/
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
public:
	ABattleGridCell();
	void AssignTrap(TObjectPtr<ABaseTrap> trap);
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
protected:
	virtual void OnCellClicked(AActor* TouchedActor, FKey ButtonPressed) override;
	UFUNCTION()
	void OnHoverOver(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void OnHoverEnd(UPrimitiveComponent* TouchedComponent);
};
