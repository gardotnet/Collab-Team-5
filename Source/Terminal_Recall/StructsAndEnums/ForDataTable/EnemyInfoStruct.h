#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EnemyInfoStruct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FEnemyInfoStruct : public FTableRowBase
{
	GENERATED_BODY()

	FEnemyInfoStruct() = default;
	
	FEnemyInfoStruct(FName name, float health , const TArray<int32>& attackList, FIntPoint size)
	{
		EnemyName = name;
		Health = health;
		AttackIdList = attackList;
		GridSize = size;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EnemyName = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> AttackIdList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint GridSize = FIntPoint(1, 1);
};
