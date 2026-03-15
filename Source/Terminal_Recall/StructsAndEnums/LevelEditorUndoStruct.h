// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelEditorBrushEnum.h"
#include "LevelExportStruct.h"
#include "For2dArray/LevelCellData.h"
#include "UObject/Object.h"
#include "LevelEditorUndoStruct.generated.h"

USTRUCT(BlueprintType)
struct FLevelEditorCellInfo
{
	GENERATED_BODY()
	
	FIntPoint GridPos = FIntPoint(-1);
	FLevelCellData CellData;
	FLevelCellData RedoCellData;
	int32 IsmIndex = -1;
	
	bool IsGridSizeChanged = false;
	FLevelExportStruct OldGridData;
	FLevelExportStruct NewGridData;
};

USTRUCT(BlueprintType)
struct FLevelEditorUndo
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FLevelEditorCellInfo> UndoInfo;
};
