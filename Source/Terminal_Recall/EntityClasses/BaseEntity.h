#pragma once

#include "CoreMinimal.h"
#include "Terminal_Recall/StructsAndEnums/MoveTypeEnum.h"
#include "BaseEntity.generated.h"

class UCapsuleComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveDone);

UCLASS()
class TERMINAL_RECALL_API ABaseEntity : public APawn
{
	GENERATED_BODY()

public:
	ABaseEntity();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(BlueprintAssignable, Category = "Events For Designers")
	FOnMoveDone OnMoveDoneEvent; // [NOTE] Make sure to use this event when player reached destination
	
	UPROPERTY(EditAnywhere, Category="Entity Info (For Designers)")
	float MaxHealth = 100;
	UPROPERTY(EditAnywhere, Category="Entity Info (For Designers)")
	EMoveTypeEnum MoveType = EMoveTypeEnum::None;
	UPROPERTY(EditAnywhere, Category="Entity Info (For Designers)", meta = (ToolTip = "How many cells entity can move in a certain direction"))
	int MoveMagnitude;

	UPROPERTY(EditAnywhere, Category="Entity Info (For Designers)", meta=(EditCondition = "false"))
	float CurrentHealth;
	
	virtual void BeginPlay() override;
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void OnMove(FVector pos);
	UFUNCTION()
	virtual void OnEntityClicked(AActor* TouchedActor, FKey ButtonPressed);
};
