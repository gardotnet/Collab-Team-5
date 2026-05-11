#pragma once

#include "CoreMinimal.h"
#include "BattleEntity.h"
#include "BattlePlayerEntity.generated.h"

class UDefaultInstance;
UCLASS()
class TERMINAL_RECALL_API ABattlePlayerEntity : public ABattleEntity
{
	GENERATED_BODY()
public:
	ABattlePlayerEntity();
	virtual void OnBattleStart(ABattleManager* battleManager) override;
	virtual void CustomTakeDamage(float damage) override;
protected:
	virtual void StartMove(FVector position) override;

	void EquipMechPart();
	bool IsMechPartUnlocked(bool bIsAttack, int32 Index) const;

	UPROPERTY()
	TObjectPtr<UDefaultInstance> GI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MouseMesh;

	//== PLAYER MECH PART SLOTS ==
	//Player Character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CharaMesh;

	//Mecha Arms
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Index")
	int32 MOVE_ArmsIndex = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Index")
	int32 ATK_ArmsIndex = -1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> R_ArmMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> L_ArmMesh;

	//Mecha Legs
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Index")
	int32 MOVE_LegsIndex = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Index")
	int32 ATK_LegsIndex = -1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> R_LegMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> L_LegMesh;

	//Mecha Boosters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Index")
	int32 MOVE_BoosterIndex = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Index")
	int32 ATK_BoosterIndex = -1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> BoostersMesh;
	
	//Mecha Cockpit (called BP_MechWings)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Index")
	int32 MOVE_WingsIndex = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Index")
	int32 ATK_WingsIndex = -1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CockpitMesh;

	//Mecha Cockpit Windshield
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> WindshieldMesh;
};
