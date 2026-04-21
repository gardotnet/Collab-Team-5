// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_WeightMapManager.h"

#include "Terminal_Recall/EntityClasses/BattlePlayerEntity.h"
#include "Terminal_Recall/GridCellClasses/BaseGridCell.h"
#include "Terminal_Recall/GridCellClasses/BattleGridCell.h"
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
	if (BattleManager == NULL)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "GatherMoveableCells failed as Weight Map has no reference to BattleManager");
		return;
	}
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

	TArray<FEntityTwinActionContainer> availableActionList = playerEntity->ActionList;

	for (FEntityTwinActionContainer action : availableActionList)
	{
		if (action.Cooldown == 0)
		{
			for (FGridRangeCellInfo range : action.AttackAction.ActionRange.Cells)
			{
				FIntPoint point = playerEntity->GetPos();
				point += range.GridPos;
				if (point.X < 0 || point.Y < 0) { continue; }
				if (point.X < BattleManager->GridSize.X || point.Y < BattleManager->GridSize.Y) { continue; }
				possibleTravelPosition.Add(range.GridPos + point);
			}
		}
	}

	GenerateWeightMap();
}

void AAI_WeightMapManager::GenerateWeightMap()
{
	for (FIntPoint pos : possibleTravelPosition)
	{
		WeightMap.Add(pos, weightSize);

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

				WeightMap.Add(nearbyCellPos, weightCellStrength);
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
			AVisualWeightMapCell* colouredSquare = GetWorld()->SpawnActor<AVisualWeightMapCell>(VisualiseWeightmap);
			if (colouredSquare == nullptr)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Failed to spawn weight map | Incorrect class! A derived blueprint class must be used from AVisualWeightMapCell in the C++ folder");
				return;
			}
			TObjectPtr<ABattleGridCell>* cell = BattleManager->GridMap.Find(FIntPoint(KeyValuePair.Key.X,KeyValuePair.Key.Y));
			if (cell == nullptr)
			{
				continue;
			}
			FVector location = (*cell)->GetActorLocation();
			location.Z = (*cell)->GetActorLocation().Z - KeyValuePair.Value * 20;
			colouredSquare->SetActorLocation(location);
			colouredSquare->chosenGradient = KeyValuePair.Value;
			colouredSquare->maxGradient = weightSize;
			colouredSquareArr.Add(colouredSquare);
		}
	}
	FIntPoint output = BattleManager->GridSize;
}

// Called every frame
void AAI_WeightMapManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

