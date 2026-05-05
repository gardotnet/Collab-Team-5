#pragma once

#include "CoreMinimal.h"
#include "GridRangeExport.h"
#include "Terminal_Recall/StatusEffect/BaseStatusEffect.h"
#include "UObject/Object.h"
#include "EntityTwinActionContainer.generated.h"

USTRUCT(BlueprintType)
struct FActionRefStruct
{
	GENERATED_BODY()
	UPROPERTY()
	FGridRangeExport ActionRange;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 CooldownDuration = -1;
	UPROPERTY()
	float Damage = 0;
	UPROPERTY()
	TSubclassOf<ABaseStatusEffect> StatusToApply = nullptr;
	UPROPERTY()
	bool bIsSelfAppliedStatus = false;
};

USTRUCT(BlueprintType)
struct FEntityTwinActionContainer 
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FActionRefStruct AttackAction;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FActionRefStruct MovementAction;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 Cooldown = -1;
};