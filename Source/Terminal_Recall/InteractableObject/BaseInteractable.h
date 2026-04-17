#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseInteractable.generated.h"

class UBoxComponent;
class ABattleEntity;
class ABattleGridCell;

UCLASS()
class TERMINAL_RECALL_API ABaseInteractable : public AActor
{
	GENERATED_BODY()

public:
	ABaseInteractable();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stuff For Designers | General Stuff")
	bool bIsPersistent = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stuff For Designers | General Info")
	bool bIsTriggered = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stuff For Designers | General Stuff")
	int Cooldown = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stuff For Designers | General Stuff")
	int CooldownTime = 0;
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stuff For Designers | General Stuff")
	ABattleGridCell* ParentGridCell;
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stuff For Designers | Functions")
	void OnLinked();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stuff For Designers | Functions")
	void OnActivate(ABattleEntity* entity);
	virtual void OnActivate_Implementation(ABattleEntity* entity);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stuff For Designers | Functions")
	void ResetInteractable();
	virtual void ResetInteractable_Implementation();

	virtual void RemoveFromGame();
};
