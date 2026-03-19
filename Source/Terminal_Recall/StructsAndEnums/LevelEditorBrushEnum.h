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
	EnemySpawn    UMETA(DisplayName = "EnemySpawn"),
	EnemyChunk	  UMETA(DisplayName = "EnemyChunk"),
	Trap          UMETA(DisplayName = "Trap"),
	PlayerSpawn	  UMETA(DisplayName = "PlayerSpawn"),
	InteractableSpawn	UMETA(DisplayName = "InteractableSpawn")
};
