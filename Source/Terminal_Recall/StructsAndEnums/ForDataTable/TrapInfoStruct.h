#pragma once

#include "CoreMinimal.h"
#include "Terminal_Recall/StructsAndEnums/For2dArray/GridRow.h"
#include "Terminal_Recall/InteractableObject/BaseInteractable.h"
#include "TrapInfoStruct.generated.h"

USTRUCT(BlueprintType)
struct FTrapInfoStruct : public FTableRowBase
{
	GENERATED_BODY()

	FTrapInfoStruct() = default;
	
	FTrapInfoStruct(FName name , const TArray<FGridRow>& size, TSubclassOf<ABaseInteractable> trapObject)
	{
		TrapName = name;
		GridSize = size;
		ObjectRef = trapObject;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TrapName = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGridRow> GridSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABaseInteractable> ObjectRef;
};