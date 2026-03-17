#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
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
struct FGridRangeCellInfo
{
	GENERATED_BODY()
	UPROPERTY()
	EGridRangeBrushMode CellType;
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
	ECompassDirection Direction;
	UPROPERTY()
	int32 Magnitude;
};
USTRUCT()
struct FGridRangeCellInfoForExport
{
	GENERATED_BODY()
	UPROPERTY()
	FIntPoint GridPos;
	UPROPERTY()
	EGridRangeBrushMode CellType;
	UPROPERTY()
	TArray<FDirectionAndMag> DirectionModifier;
};

USTRUCT()
struct FGridRangeExport
{
	GENERATED_BODY()
	UPROPERTY()
	FIntPoint CentrePoint;
	UPROPERTY()
	TArray<FGridRangeCellInfoForExport> Cells;
};
