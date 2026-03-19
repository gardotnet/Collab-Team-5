#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AssetLookUpStruct.generated.h"

USTRUCT(BlueprintType)
struct FAssetLookUpTable : public FTableRowBase
{
	GENERATED_BODY()
	
	FAssetLookUpTable() = default;
	FAssetLookUpTable(FName name, TSubclassOf<AActor> ActorRef)
	{
		AssetName = name;
		ActorBP = ActorRef;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AssetName = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ActorBP;
};
