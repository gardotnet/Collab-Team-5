#pragma once

#include "CoreMinimal.h"
#include "ActionStruct.generated.h"

USTRUCT(BlueprintType)
struct FActionStruct : public FTableRowBase
{
	GENERATED_BODY()

	FActionStruct() = default;
	
	FActionStruct(FName name, float dmgMult , FString attackRange)
	{
		ActionName = name;
		Damage = dmgMult;
		GridRangeFilePath = attackRange;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ActionName = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GridRangeFilePath = "";
};
