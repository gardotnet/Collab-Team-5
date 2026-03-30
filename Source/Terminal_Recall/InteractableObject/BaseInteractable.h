#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseInteractable.generated.h"

UCLASS()
class TERMINAL_RECALL_API ABaseInteractable : public AActor
{
	GENERATED_BODY()

public:
	ABaseInteractable();

protected:
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable|Interaction")
	void OnActivate();
	virtual void OnActivate_Implementation();
};
