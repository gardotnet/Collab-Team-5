#pragma once

#include "CoreMinimal.h"
#include "GridRangeExport.generated.h"

UENUM()
enum class EGridRangeBrushMode : int32
{
	None,
	Erase,
	Slot,
	Centre,
	Select,
};

UENUM()
enum class ECompassDirection : int32
{
	None,
	N,
	NE,
	E,
	SE,
	S,
	SW,
	W,
	NW
};

USTRUCT()
struct FEditorGridRangeCellState
{
	GENERATED_BODY()
	UPROPERTY()
	EGridRangeBrushMode CellType = EGridRangeBrushMode::None;
	UPROPERTY()
	int32 N = -1;
	int32 NE = -1;
	int32 E = -1;
	int32 SE = -1;
	int32 S = -1;
	int32 SW = -1;
	int32 W = -1;
	int32 NW= -1;
	void ClearModifier()
	{
		N = -1;
		NE = -1;
		E = -1;
		SE = -1;
		S = -1;
		SW = -1;
		W = -1;
		NW= -1;
	}
};

USTRUCT()
struct FDirectionAndMag
{
	GENERATED_BODY()
	UPROPERTY()
	ECompassDirection Direction = ECompassDirection::None;
	UPROPERTY()
	int32 Magnitude = -1;
};
USTRUCT()
struct FGridRangeCellInfo
{
	GENERATED_BODY()
	UPROPERTY()
	FIntPoint GridPos = FIntPoint(-1,-1);
	UPROPERTY()
	TArray<FDirectionAndMag> DirectionModifier;
};

USTRUCT()
struct FGridRangeExport : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY()
	FIntPoint CentrePoint = FIntPoint(-1,-1);
	UPROPERTY()
	TArray<FGridRangeCellInfo> Cells;
};
