#pragma once

#include "CoreMinimal.h"
#include "Terminal_Recall/StructsAndEnums/LevelEditorBrushEnum.h"
#include "UObject/Object.h"
#include "LevelCellData.generated.h"

USTRUCT(BlueprintType)
struct FLevelCellData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECellBrushMode CellType = ECellBrushMode::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UObject> RequiredObject;
};
