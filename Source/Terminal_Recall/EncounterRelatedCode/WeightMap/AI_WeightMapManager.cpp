// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_WeightMapManager.h"

#include "Terminal_Recall/EntityClasses/BattlePlayerEntity.h"
#include "WorldPartition/WorldPartitionBuilder.h"


// Sets default values
AAI_WeightMapManager::AAI_WeightMapManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAI_WeightMapManager::BeginPlay()
{
	Super::BeginPlay();
	
	GatherMoveableCells();
}

void AAI_WeightMapManager::GatherMoveableCells()
{
	FIntPoint output = BattleManager->GridSize;

	for (int i = 0; i < output.X; i++)
	{
		for (int j = 0; j < output.Y; j++)
		{
			TObjectPtr<ABattleGridCell>* cell = BattleManager->GridMap.Find(FIntPoint(i,j));
			if (cell == nullptr)
			{
				continue;
			}
			cellMap.Add(FIntPoint(i,j), cell);

			WeightMap.Add(FIntPoint(i,j), NULL);
		}
	}

	GetCurrentActionPositionsForPlayer();
}

void AAI_WeightMapManager::GetCurrentActionPositionsForPlayer()
{
	TObjectPtr<ABattlePlayerEntity> playerEntity = BattleManager->PlayerRef;

	FIntPoint playerPosition = playerEntity->GetPos();
	TArray<FEntityTwinActionContainer> availableActionList = playerEntity->ActionList;

	

	for (FEntityTwinActionContainer action : availableActionList)
	{
		if (action.Cooldown == 0)
		{
			for (FGridRangeCellInfo range : action.AttackAction.ActionRange.Cells)
			{
				possibleTravelPosition.Add(range.GridPos);
			}
		}
	}
}

void AAI_WeightMapManager::GenerateWeightMap()
{
	for (FIntPoint pos : possibleTravelPosition)
	{
		WeightMap[pos] = weightSize;

		for (int i = 0; i < weightSize; i++)
		{
			for (int j = 0; j < weightSize; j++)
			{
				FIntPoint nearbyCellPos = pos + FIntPoint(i,j);

				int weightCellStrength = 0;

				if (weightSize - i < weightSize - j)
				{
					weightCellStrength = weightSize - i;
				}
				else
				{
					weightCellStrength = weightSize - j;
				}

				if (weightSize - i <= 1 && weightSize - j <= 1)
				{
					continue;
				}
				
				WeightMap[nearbyCellPos] = weightCellStrength;
			}
		}
	}

	ShowWeightMap();
}

void AAI_WeightMapManager::ShowWeightMap()
{
	if (bshowWeightMap)
	{
		for (const TTuple<FIntPoint, int> KeyValuePair : WeightMap)
		{
			AActor* colouredSquare = GetWorld()->SpawnActor<AActor>();
			FVector Pos = FVector{static_cast<float>(KeyValuePair.Key.X), static_cast<float>(KeyValuePair.Key.Y), 0.2};
			colouredSquare->SetActorLocation(Pos);
		}
	}
}

// Called every frame
void AAI_WeightMapManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

