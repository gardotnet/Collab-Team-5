#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EnemyInfoStruct.generated.h"

class ABattleEnemyEntity;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FEnemyInfoStruct : public FTableRowBase
{
	GENERATED_BODY()
	FEnemyInfoStruct() = default;
	FEnemyInfoStruct(FName name, float health , const TArray<int32>& attackList, const TArray<int32>& moveList, int32 moveDist, FIntPoint size, UTexture2D)
	{
		EnemyName = name;
		Health = health;
		AttackIdList = attackList;
		MovementIdList = moveList;
		MaxMoveDistance = moveDist;
		GridSize = size;
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EnemyName = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> AttackIdList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> MovementIdList = { 1 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxMoveDistance = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint GridSize = FIntPoint(1, 1);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABattleEnemyEntity> EnemyBpRef = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> EnemyIconTexture = nullptr;
};
