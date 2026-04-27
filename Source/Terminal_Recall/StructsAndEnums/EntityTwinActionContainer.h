#pragma once

#include "CoreMinimal.h"
#include "GridRangeExport.h"
#include "UObject/Object.h"
#include "EntityTwinActionContainer.generated.h"

USTRUCT(BlueprintType) // Added BlueprintType to make it accessible in Blueprints
struct FActionRefStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Info")
	FName ActionName; // Added ActionName property

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Info") // Changed to EditAnywhere/BlueprintReadWrite
	FGridRangeExport ActionRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Info") // Changed to EditAnywhere/BlueprintReadWrite
	int32 CooldownDuration = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Info") // Changed to EditAnywhere/BlueprintReadWrite
	float Damage = 0;
};

USTRUCT(BlueprintType) // Added BlueprintType to make it accessible in Blueprints
struct FEntityTwinActionContainer 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Info") // Changed to EditAnywhere/BlueprintReadWrite
	FActionRefStruct AttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Info") // Changed to EditAnywhere/BlueprintReadWrite
	FActionRefStruct MovementAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Info") // Changed to EditAnywhere/BlueprintReadWrite
	int32 Cooldown = -1;
};
