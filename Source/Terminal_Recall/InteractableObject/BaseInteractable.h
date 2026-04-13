#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseInteractable.generated.h"

class ABattleEntity;
class ABattleGridCell;

UCLASS()
class TERMINAL_RECALL_API ABaseInteractable : public AActor
{
	GENERATED_BODY()

public:
	ABaseInteractable();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stuff For Designers | General Stuff")
	bool bIsPersistent = false;
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stuff For Designers | General Stuff")
	ABattleGridCell* ParentGridCell;
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stuff For Designers | Functions")
	void OnLinked();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stuff For Designers | Functions")
	void OnActivate(ABattleEntity* entity);
	virtual void OnActivate_Implementation(ABattleEntity* entity);

	virtual void RemoveFromGame();
};
