#pragma once

#include "CoreMinimal.h"
#include "Terminal_Recall/InteractableObject/BaseInteractable.h"
#include "LevelnfoStruct.generated.h"

USTRUCT(BLueprintType)
struct FLevelInfoStruct: public FTableRowBase
{
	GENERATED_BODY()
	FLevelInfoStruct() = default;
	FLevelInfoStruct(FString name, int32 difficulty,TArray<int32> enemyIDs, FString filePath )
	{
		LevelName = name;
		Difficulty = difficulty;
		EnemyIdForHints = enemyIDs;
		LevelFilePath = filePath;
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LevelName = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Difficulty = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> EnemyIdForHints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LevelFilePath = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABaseInteractable> RobotPickupBP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint PartSpawnLoc = FIntPoint(0,0);
};
