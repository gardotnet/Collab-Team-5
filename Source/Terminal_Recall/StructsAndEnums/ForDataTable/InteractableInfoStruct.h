#pragma once

#include "CoreMinimal.h"
#include "Terminal_Recall/InteractableObject/BaseItemInteractable.h"
#include "UObject/Object.h"
#include "InteractableInfoStruct.generated.h"

USTRUCT(BlueprintType)
struct FInteractableInfoStruct : public FTableRowBase
{
	GENERATED_BODY()

	FInteractableInfoStruct() = default;
	
	FInteractableInfoStruct(FName name, TSubclassOf<ABaseItemInteractable> interactableObject)
	{
		InteractableName = name;
		ObjectRef = interactableObject;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName InteractableName = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABaseItemInteractable> ObjectRef;
};