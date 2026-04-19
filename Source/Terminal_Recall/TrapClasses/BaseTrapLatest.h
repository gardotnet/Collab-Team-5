#pragma once

#include "CoreMinimal.h"
#include "Terminal_Recall/InteractableObject/BaseInteractable.h"
#include "BaseTrapLatest.generated.h"

UCLASS(Blueprintable, BlueprintType)
class TERMINAL_RECALL_API ABaseTrapLatest : public ABaseInteractable
{
	GENERATED_BODY()

public:
	ABaseTrapLatest();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable|Info")
	int32 TrapDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable|Info")
	bool bIsTriggered = false;

	virtual void ResetTrap();

	virtual void OnActivate_Implementation(ABattleEntity* entity) override;

	virtual void RemoveFromGame() override;
};
