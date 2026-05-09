#pragma once

#include "CoreMinimal.h"
#include "BattleEntity.h"
#include "BattlePlayerEntity.generated.h"



UCLASS()
class TERMINAL_RECALL_API ABattlePlayerEntity : public ABattleEntity
{
	GENERATED_BODY()
public:
	ABattlePlayerEntity();
	virtual void OnBattleStart(ABattleManager* battleManager) override;
	virtual void CustomTakeDamage(float damage) override;
protected:
	virtual void StartMove(FVector position) override;;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MouseMesh;

	//== PLAYER MECH PART SLOTS ==
	//Player Character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CharaMesh;

	//Mecha Arms
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> R_ArmMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> L_ArmMesh;

	//Mecha Legs
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> L_LegMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> R_LegMesh;

	//Mecha Boosters
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> BoostersMesh;
	
	//Mecha Cockpit (called BP_MechWings)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CockpitMesh;

	//Mecha Cockpit Windshield
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> WindshieldMesh;
};
