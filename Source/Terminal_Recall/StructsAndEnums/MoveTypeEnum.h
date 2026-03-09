#pragma once

#include "CoreMinimal.h"
#include "MoveTypeEnum.generated.h"

UENUM(BlueprintType)
enum class EMoveTypeEnum : uint8
{
	None UMETA(DisplayName = "None"),
	King UMETA(DisplayName = "King", ToolTip = "8 way movement"),
	Bishop UMETA(DisplayName = "Bishop", ToolTip = "Diagonal movement"),
	Rook UMETA(DisplayName = "Rook", ToolTip = "Orthogonal movement"),
};
