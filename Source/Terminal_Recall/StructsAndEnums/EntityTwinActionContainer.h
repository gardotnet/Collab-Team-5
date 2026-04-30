#pragma once

#include "CoreMinimal.h"
#include "GridRangeExport.h"
#include "Terminal_Recall/StatusEffect/BaseStatusEffect.h"
#include "UObject/Object.h"
#include "EntityTwinActionContainer.generated.h"

USTRUCT()
struct FActionRefStruct
{
	GENERATED_BODY()
	UPROPERTY()
	FGridRangeExport ActionRange;
	UPROPERTY()
	int32 CooldownDuration = -1;
	UPROPERTY()
	float Damage = 0;
	UPROPERTY()
	TSubclassOf<ABaseStatusEffect> StatusToApply = nullptr;
	UPROPERTY()
	bool bIsSelfAppliedStatus = false;
};

USTRUCT()
struct FEntityTwinActionContainer 
{
	GENERATED_BODY()

	UPROPERTY()
	FActionRefStruct AttackAction;
	UPROPERTY()
	FActionRefStruct MovementAction;
	UPROPERTY()
	int32 Cooldown = -1;
};


