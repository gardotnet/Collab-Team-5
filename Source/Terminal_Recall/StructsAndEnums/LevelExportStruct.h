#pragma once

#include "CoreMinimal.h"
#include "LevelEditorBrushEnum.h"
#include "UObject/Object.h"
#include "LevelExportStruct.generated.h"

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
};

USTRUCT(BlueprintType)
struct FLevelExportStruct
{
	GENERATED_BODY()
	UPROPERTY()
	FIntPoint GridSize = FIntPoint(-1, -1);
	UPROPERTY()
	TArray<FExportCellDetails> GridCells;
};
