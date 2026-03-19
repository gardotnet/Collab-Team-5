#pragma once

#include "CoreMinimal.h"
#include "Terminal_Recall/StructsAndEnums/For2dArray/GridRow.h"
#include "UObject/Object.h"
#include "TrapInfoStruct.generated.h"

USTRUCT(BlueprintType)
struct FTrapInfoStruct : public FTableRowBase
{
	GENERATED_BODY()

	FTrapInfoStruct() = default;
	
	FTrapInfoStruct(FName name , const TArray<FGridRow>& size, TObjectPtr<AActor> trapObject)
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
	TObjectPtr<AActor> ObjectRef;
};