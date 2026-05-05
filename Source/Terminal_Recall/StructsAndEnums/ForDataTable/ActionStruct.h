#pragma once

#include "CoreMinimal.h"
#include "Terminal_Recall/StatusEffect/BaseStatusEffect.h"
#include "ActionStruct.generated.h"

USTRUCT(BlueprintType)
struct FActionStruct : public FTableRowBase
{
	GENERATED_BODY()
	FActionStruct() = default;
	FActionStruct(FName name, float dmgMult , FString attackRange, int32 cooldown)
	{
		ActionName = name;
		Damage = dmgMult;
		GridRangeFilePath = attackRange;
		CooldownDuration = cooldown;
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ActionName = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GridRangeFilePath = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CooldownDuration = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsSelfAppliedStatus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABaseStatusEffect> StatusEffect;
	
};
