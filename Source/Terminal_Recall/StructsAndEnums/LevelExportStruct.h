#pragma once

#include "CoreMinimal.h"
#include "LevelEditorBrushEnum.h"
#include "LevelExportStruct.generated.h"

USTRUCT(BlueprintType)
struct FGridPosAnd1DIndex
{
	GENERATED_BODY()
	UPROPERTY()
	FIntPoint GridPos = FIntPoint(-1);
	UPROPERTY()
	int32 IndexPos = -1;
};

USTRUCT(BlueprintType)
struct FExportCellDetails
{
	GENERATED_BODY()
	UPROPERTY()
	FIntPoint Position = FIntPoint(-1,-1);
	UPROPERTY()
	ECellBrushMode CellType = ECellBrushMode::None;
	UPROPERTY()
	int32 IdForCellContent = -1;
	
	bool operator==(const FExportCellDetails& Other) const
	{
		return CellType == Other.CellType && 
			   IdForCellContent == Other.IdForCellContent &&
			   	Position == Other.Position; 
	}
};

USTRUCT(BlueprintType)
struct FLevelExportStruct
{
	GENERATED_BODY()
	UPROPERTY()
	FIntPoint GridSize = FIntPoint(-1, -1);
	UPROPERTY()
	TArray<FExportCellDetails> GridCells;
	
	bool operator==(const FLevelExportStruct& Other) const
	{
		return GridSize == Other.GridSize &&
			GridCells == Other.GridCells;
	}
};
