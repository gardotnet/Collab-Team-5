#pragma once

#include "CoreMinimal.h"
#include "GridRow.h"
#include "AttackStruct.generated.h"

USTRUCT(BlueprintType)
struct FAttackStruct : public FTableRowBase
{
	GENERATED_BODY()

	FAttackStruct() = default;
	
	FAttackStruct(FName name, float dmgMult , const TArray<FGridRow>& attackRange)
	{
		AttackName = name;
		DamageMultiplier = dmgMult;
		ShapeRange = attackRange;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AttackName = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageMultiplier = 1.0;
	// [NOTE] This works but not visual good add a way so that you can see it like this in editor
	// { 0, 0, 1}
	// { 0, 0, 0}
	// { 0, 1, 0}
	// not really necessary but helps people visualise the attack range better
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGridRow> ShapeRange;
};
