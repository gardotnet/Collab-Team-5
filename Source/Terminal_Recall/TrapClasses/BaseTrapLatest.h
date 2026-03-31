#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
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
	
	virtual void OnActivate_Implementation() override;
};
