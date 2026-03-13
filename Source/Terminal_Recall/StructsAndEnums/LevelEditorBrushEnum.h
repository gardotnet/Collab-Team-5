#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LevelEditorBrushEnum.generated.h"

UENUM(BlueprintType)
enum class ECellBrushMode : uint8
{
	None          UMETA(DisplayName = "None"),
	Walkable      UMETA(DisplayName = "Walkable"),
	Wall          UMETA(DisplayName = "Wall"),
	EnemySpawn    UMETA(DisplayName = "Enemy Spawn"),
	Trap          UMETA(DisplayName = "Trap"),
	PlayerSpawn	  UMETA(DisplayName = "Player Spawn"),
};
