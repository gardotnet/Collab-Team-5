#pragma once

#include "CoreMinimal.h"
#include "GridRangeExport.generated.h"

UENUM()
enum class EGridRangeBrushMode : int8
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
	None = -1,
	N = 0,
	NE = 1,
	E = 2,
	SE = 3,
	S = 4,
	SW = 5,
	W = 6,
	NW = 7
};

USTRUCT()
struct FEditorGridRangeCellState
{
	GENERATED_BODY()
	UPROPERTY()
	EGridRangeBrushMode CellType = EGridRangeBrushMode::None;
	TMap<ECompassDirection, int32> DirectionMags;
	int32 GroupID = -1;
	FEditorGridRangeCellState()
	{
		GroupID = -1;
		CellType = EGridRangeBrushMode::None;
		DirectionMags.Add(ECompassDirection::N, -1);
		DirectionMags.Add(ECompassDirection::NE, -1);
		DirectionMags.Add(ECompassDirection::E, -1);
		DirectionMags.Add(ECompassDirection::SE, -1);
		DirectionMags.Add(ECompassDirection::S, -1);
		DirectionMags.Add(ECompassDirection::SW, -1);
		DirectionMags.Add(ECompassDirection::W, -1);
		DirectionMags.Add(ECompassDirection::NW, -1);
	}
	void ClearModifier()
	{
		for (auto& direction : DirectionMags )
		{
			direction.Value = -1;
		}
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
	UPROPERTY()
	int32 GroupID = -1;
};

USTRUCT()
struct FGridRangeExport : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY()
	FIntPoint CentrePoint = FIntPoint(-1,-1);
	UPROPERTY()
	TArray<FGridRangeCellInfo> Cells;
	UPROPERTY()
	FIntPoint GridSize = FIntPoint(-1,-1);
	UPROPERTY()
	bool OneGroup = false;
};
