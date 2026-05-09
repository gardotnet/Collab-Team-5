// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "WorldTraversingEntityV2.generated.h"

class UDefaultInstance;
class USplineComponent;
class ATraversalPlayerController;
class AHarryWorldTraversalNode;

UCLASS()
class TERMINAL_RECALL_API AWorldTraversingEntityV2 : public ABaseEntity
{
	GENERATED_BODY()

public:
	AWorldTraversingEntityV2();
	
protected:
	
	UPROPERTY()
	TObjectPtr<ATraversalPlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Stuff For Designers | Variables")
	TArray<TObjectPtr<AHarryWorldTraversalNode>> TheNodePath;
	int32 PathIndex;
	
	UPROPERTY()
	TObjectPtr<USplineComponent> CurrentSplineComp;
	bool bDontReverseSplineLoc = true;
	
	FRotator TargetRotation;
	
	TArray<TObjectPtr<AHarryWorldTraversalNode>> TheReversePath;
	UPROPERTY()
	UDefaultInstance* TheGameInstance;

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

public:
	virtual void StartMove(TArray<TObjectPtr<AHarryWorldTraversalNode>> path) override;
	
	void MoveBack();

protected:
	
	virtual void BeginPlay() override;
	virtual void OnMove() override;
	
	UFUNCTION()
	virtual void OnSplineTraversal();

	void EquipMechPart();
};
