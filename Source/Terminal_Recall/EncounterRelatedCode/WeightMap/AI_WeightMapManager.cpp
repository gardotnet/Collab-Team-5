// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_WeightMapManager.h"

#include "Terminal_Recall/EntityClasses/BattlePlayerEntity.h"
#include "Terminal_Recall/GridCellClasses/BaseGridCell.h"
#include "Terminal_Recall/GridCellClasses/BattleGridCell.h"
#include "WorldPartition/WorldPartitionBuilder.h"
#include "DrawDebugHelpers.h" // Required for DrawDebugBox and DrawDebugString


// Sets default values
AAI_WeightMapManager::AAI_WeightMapManager()
{
	// Set this actor to call Tick() every frame.  You can turn off this to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LastPlayerPosition = FIntPoint(-1, -1); // Initialize with an invalid position
}

// Called when the game starts or when spawned
void AAI_WeightMapManager::BeginPlay()
{
	Super::BeginPlay();
	
	GatherMoveableCells();

	// After initial generation, store the player's current position
	if (BattleManager && BattleManager->PlayerRef)
	{
		LastPlayerPosition = BattleManager->PlayerRef->GetPos();
	}
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

			WeightMap.Add(FIntPoint(i,j), 0); // Initialize with 0 instead of NULL
		}
	}

	GetCurrentActionPositionsForPlayer();
}

// Helper function to get FIntPoint offset from ECompassDirection
FIntPoint AAI_WeightMapManager::GetDirectionOffset(ECompassDirection Direction)
{
	switch (Direction)
	{
	case ECompassDirection::N: return FIntPoint(0, 1);
	case ECompassDirection::NE: return FIntPoint(1, 1);
	case ECompassDirection::E: return FIntPoint(1, 0);
	case ECompassDirection::SE: return FIntPoint(1, -1);
	case ECompassDirection::S: return FIntPoint(0, -1);
	case ECompassDirection::SW: return FIntPoint(-1, -1);
	case ECompassDirection::W: return FIntPoint(-1, 0);
	case ECompassDirection::NW: return FIntPoint(-1, 1);
	default: return FIntPoint(0, 0); // None or invalid
	}
}


void AAI_WeightMapManager::GetCurrentActionPositionsForPlayer()
{
	// Clear previous possible travel positions before recalculating
	possibleAttackTargetActions.Empty(); // Changed to clear the new map

	TObjectPtr<ABattlePlayerEntity> playerEntity = BattleManager->PlayerRef;

	if (!playerEntity)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "GetCurrentActionPositionsForPlayer failed: PlayerRef is null.");
		return;
	}

	TArray<FEntityTwinActionContainer> availableActionList = playerEntity->ActionList;

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("Total available actions: %d"), availableActionList.Num()));

	FIntPoint playerPos = playerEntity->GetPos(); // Get player position once per call
	FVector playerWorldLocation = FVector::ZeroVector;

	// Get player's world location for drawing debug arrows
	TObjectPtr<ABattleGridCell>** playerCellPtr = cellMap.Find(playerPos);
	if (playerCellPtr && *playerCellPtr)
	{
		playerWorldLocation = (**playerCellPtr)->GetActorLocation();
	}


	for (FEntityTwinActionContainer action : availableActionList)
	{
		if (action.Cooldown == 0)
		{
			// --- Process Attack Action ---
			int attackCellsProcessed = 0;
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Magenta, FString::Printf(TEXT("Processing Attack Action from container (Cooldown: %d)"), action.Cooldown));

			// Generate a descriptive string for the current action, including the new ActionName
			FString attackActionDescription = FString::Printf(TEXT("%s (CD:%d, Dmg:%.0f)"), *action.AttackAction.ActionName.ToString(), action.Cooldown, action.AttackAction.Damage);

			for (FGridRangeCellInfo range : action.AttackAction.ActionRange.Cells)
			{
				if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("  Attack: Processing FGridRangeCellInfo. GridPos: (%d,%d)"), range.GridPos.X, range.GridPos.Y));

				// Calculate the initial target position from playerPos and range.GridPos
				FIntPoint initialTargetPos = playerPos + range.GridPos;

				// Process the initial target position
				if (initialTargetPos.X >= 0 && initialTargetPos.Y >= 0 &&
					initialTargetPos.X < BattleManager->GridSize.X && initialTargetPos.Y < BattleManager->GridSize.Y)
				{
					possibleAttackTargetActions.FindOrAdd(initialTargetPos).Descriptions.AddUnique(attackActionDescription);
					if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("    Attack: Added action '%s' to initial target position: (%d, %d)"), *attackActionDescription, initialTargetPos.X, initialTargetPos.Y));
					attackCellsProcessed++;

					// Draw debug arrow for initial attack range if enabled
					if (bShowAttackRangeDebug)
					{
						TObjectPtr<ABattleGridCell>** targetCellPtr = cellMap.Find(initialTargetPos);
						if (targetCellPtr && *targetCellPtr)
						{
							FVector targetWorldLocation = (**targetCellPtr)->GetActorLocation();
							FVector arrowStart = playerWorldLocation + FVector(0, 0, 50.0f);
							FVector arrowEnd = targetWorldLocation + FVector(0, 0, 50.0f);
							DrawDebugDirectionalArrow(GetWorld(), arrowStart, arrowEnd, 300.0f, FColor::Red, true, -1.0f, 0, 5.0f);
							if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Attack: Drawing arrow from (%d,%d) to (%d,%d)"), playerPos.X, playerPos.Y, initialTargetPos.X, initialTargetPos.Y));
						}
						else
						{
							if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Attack: Skipping arrow draw: Target cell (%d,%d) not found in cellMap."), initialTargetPos.X, initialTargetPos.Y));
						}
					}
				}
				else
				{
					if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("    Attack: Skipping out-of-bounds initial target position: (%d, %d)"), initialTargetPos.X, initialTargetPos.Y));
				}

				// Now process directional modifiers for extended range
				if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("  Attack: DirectionModifier count for (%d,%d): %d"), range.GridPos.X, range.GridPos.Y, range.DirectionModifier.Num()));

				for (const FDirectionAndMag& dirMag : range.DirectionModifier)
				{
					FString DirectionEnumString = UEnum::GetValueAsString(dirMag.Direction);
					if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("    Attack: Processing DirMag: Direction=%s, Magnitude=%d"), *DirectionEnumString, dirMag.Magnitude));

					if (dirMag.Direction == ECompassDirection::None || dirMag.Magnitude <= 0)
					{
						if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("    Attack: Skipping DirMag: Invalid Direction or Magnitude (None or <= 0)."));
						continue;
					}

					FIntPoint directionOffset = GetDirectionOffset(dirMag.Direction);
					if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("    Attack: GetDirectionOffset(%s) returned: (%d,%d)"), *DirectionEnumString, directionOffset.X, directionOffset.Y));

					FIntPoint currentLinePos = initialTargetPos; // Start from the initial target position

					for (int k = 1; k <= dirMag.Magnitude; k++)
					{
						currentLinePos += directionOffset; // Move one step in the direction
						if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("      Attack: Step %d: currentLinePos = (%d,%d)"), k, currentLinePos.X, currentLinePos.Y));

						// Check if currentLinePos is within grid bounds
						if (currentLinePos.X >= 0 && currentLinePos.Y >= 0 &&
							currentLinePos.X < BattleManager->GridSize.X && currentLinePos.Y < BattleManager->GridSize.Y)
						{
							possibleAttackTargetActions.FindOrAdd(currentLinePos).Descriptions.AddUnique(attackActionDescription);
							if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("        Attack: Added action '%s' to extended target position: (%d, %d) (Mag: %d)"), *attackActionDescription, currentLinePos.X, currentLinePos.Y, k));
							attackCellsProcessed++;

							// Draw debug arrow for extended attack range if enabled
							if (bShowAttackRangeDebug)
							{
								TObjectPtr<ABattleGridCell>** targetCellPtr = cellMap.Find(currentLinePos);
								if (targetCellPtr && *targetCellPtr)
								{
									FVector targetWorldLocation = (**targetCellPtr)->GetActorLocation();
									FVector arrowStart = playerWorldLocation + FVector(0, 0, 50.0f);
									FVector arrowEnd = targetWorldLocation + FVector(0, 0, 50.0f);
									DrawDebugDirectionalArrow(GetWorld(), arrowStart, arrowEnd, 300.0f, FColor::Red, true, -1.0f, 0, 5.0f);
									if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Attack: Drawing arrow from (%d,%d) to (%d,%d) (Extended)"), playerPos.X, playerPos.Y, currentLinePos.X, currentLinePos.Y));
								}
								else
								{
									if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Attack: Skipping arrow draw: Extended target cell (%d,%d) not found in cellMap."), currentLinePos.X, currentLinePos.Y));
								}
							}
						}
						else
						{
							if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("        Attack: Skipping out-of-bounds extended target position: (%d, %d) (Mag: %d)"), currentLinePos.X, currentLinePos.Y, k));
							break; // Stop extending if out of bounds
						}
					}
				}
			}
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(TEXT("Attack Action processed %d cells. Total possibleAttackTargetActions: %d"), attackCellsProcessed, possibleAttackTargetActions.Num()));

			// --- Process Movement Action ---
			int moveCellsProcessed = 0;
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Magenta, FString::Printf(TEXT("Processing Movement Action from container (Cooldown: %d)"), action.Cooldown));
			FString moveActionDescription = FString::Printf(TEXT("%s (Move)"), *action.MovementAction.ActionName.ToString());

			for (FGridRangeCellInfo range : action.MovementAction.ActionRange.Cells)
			{
				if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("  Move: Processing FGridRangeCellInfo. GridPos: (%d,%d)"), range.GridPos.X, range.GridPos.Y));

				FIntPoint initialTargetPos = playerPos + range.GridPos;

				if (initialTargetPos.X >= 0 && initialTargetPos.Y >= 0 &&
					initialTargetPos.X < BattleManager->GridSize.X && initialTargetPos.Y < BattleManager->GridSize.Y)
				{
					possibleAttackTargetActions.FindOrAdd(initialTargetPos).Descriptions.AddUnique(moveActionDescription);
					if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("    Move: Added action '%s' to initial target position: (%d, %d)"), *moveActionDescription, initialTargetPos.X, initialTargetPos.Y));
					moveCellsProcessed++;
				}
				else
				{
					if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("    Move: Skipping out-of-bounds initial target position: (%d, %d)"), initialTargetPos.X, initialTargetPos.Y));
				}

				if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("  Move: DirectionModifier count for (%d,%d): %d"), range.GridPos.X, range.GridPos.Y, range.DirectionModifier.Num()));

				for (const FDirectionAndMag& dirMag : range.DirectionModifier)
				{
					FString DirectionEnumString = UEnum::GetValueAsString(dirMag.Direction);
					if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("    Move: Processing DirMag: Direction=%s, Magnitude=%d"), *DirectionEnumString, dirMag.Magnitude));

					if (dirMag.Direction == ECompassDirection::None || dirMag.Magnitude <= 0)
					{
						if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("    Move: Skipping DirMag: Invalid Direction or Magnitude (None or <= 0)."));
						continue;
					}

					FIntPoint directionOffset = GetDirectionOffset(dirMag.Direction);
					if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("    Move: GetDirectionOffset(%s) returned: (%d,%d)"), *DirectionEnumString, directionOffset.X, directionOffset.Y));

					FIntPoint currentLinePos = initialTargetPos;

					for (int k = 1; k <= dirMag.Magnitude; k++)
					{
						currentLinePos += directionOffset;
						if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("      Move: Step %d: currentLinePos = (%d,%d)"), k, currentLinePos.X, currentLinePos.Y));

						if (currentLinePos.X >= 0 && currentLinePos.Y >= 0 &&
							currentLinePos.X < BattleManager->GridSize.X && currentLinePos.Y < BattleManager->GridSize.Y)
						{
							possibleAttackTargetActions.FindOrAdd(currentLinePos).Descriptions.AddUnique(moveActionDescription);
							if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("        Move: Added action '%s' to extended target position: (%d, %d) (Mag: %d)"), *moveActionDescription, currentLinePos.X, currentLinePos.Y, k));
							moveCellsProcessed++;
						}
						else
						{
							if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("        Move: Skipping out-of-bounds extended target position: (%d, %d) (Mag: %d)"), currentLinePos.X, currentLinePos.Y, k));
							break;
						}
					}
				}
			}
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(TEXT("Movement Action processed %d cells. Total actions in map: %d"), moveCellsProcessed, possibleAttackTargetActions.Num()));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Orange, FString::Printf(TEXT("Skipping Actions from container due to cooldown: %d"), action.Cooldown));
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("Final possibleAttackTargetActions count: %d"), possibleAttackTargetActions.Num()));

	GenerateWeightMap();
	ShowAttackFromPositionsDebug(); // Call the new debug function here
}

void AAI_WeightMapManager::GenerateWeightMap()
{
	WeightMap.Empty();

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("--- Generating WeightMap ---"));

	// Iterate through the keys of the new map
	for (const TPair<FIntPoint, FActionDescriptionArray>& Entry : possibleAttackTargetActions)
	{
		FIntPoint pos = Entry.Key; // Get the FIntPoint key

		if (cellMap.Contains(pos))
		{
			int32* existingWeightPtr = WeightMap.Find(pos);
			int32 oldWeight = existingWeightPtr ? *existingWeightPtr : 0;
			int32 newWeight = weightSize;

			if (newWeight > oldWeight)
			{
				WeightMap.FindOrAdd(pos) = newWeight;
				if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Cell (%d, %d): Weight updated from %d to %d (center)"), pos.X, pos.Y, oldWeight, newWeight));
			}
		}

		for (int i = -weightSize; i <= weightSize; i++)
		{
			for (int j = -weightSize; j <= weightSize; j++)
			{
				if (i == 0 && j == 0)
				{
					continue;
				}

				FIntPoint nearbyCellPos = pos + FIntPoint(i, j);

				if (nearbyCellPos.X >= 0 && nearbyCellPos.Y >= 0 &&
					nearbyCellPos.X < BattleManager->GridSize.X && nearbyCellPos.Y < BattleManager->GridSize.Y)
				{
					int distance = FMath::Abs(i) + FMath::Abs(j);

					if (distance > weightSize)
					{
						continue;
					}

					int currentWeight = weightSize - distance;

					int* existingWeight = WeightMap.Find(nearbyCellPos);
					if (existingWeight == nullptr || currentWeight > *existingWeight)
					{
						if (cellMap.Contains(nearbyCellPos))
						{
							int32 oldWeight = existingWeight ? *existingWeight : 0;
							WeightMap.FindOrAdd(nearbyCellPos) = currentWeight;
							if (bLogVerboseGeneration) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Cell (%d, %d): Weight updated from %d to %d"), nearbyCellPos.X, nearbyCellPos.Y, oldWeight, currentWeight));
						}
					}
				}
			}
		}
	}

	if (bLogFinalWeightMap) // Conditional log
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Purple, TEXT("--- Final WeightMap Content ---"));
		for (const TTuple<FIntPoint, int> KeyValuePair : WeightMap)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Purple, FString::Printf(TEXT("Cell (%d, %d): Weight = %d"), KeyValuePair.Key.X, KeyValuePair.Key.Y, KeyValuePair.Value));
		}
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Purple, TEXT("-----------------------------"));
	}


	ShowWeightMap();
}

void AAI_WeightMapManager::ShowWeightMap()
{
	FlushPersistentDebugLines(GetWorld());

	if (bshowWeightMap)
	{
		for (const TTuple<FIntPoint, int> KeyValuePair : WeightMap)
		{
			TObjectPtr<ABattleGridCell>** cell = cellMap.Find(FIntPoint(KeyValuePair.Key.X, KeyValuePair.Key.Y));
			if (cell == nullptr || *cell == nullptr)
			{
				continue;
			}

			FVector cellLocation = (**cell)->GetActorLocation();
			int weight = KeyValuePair.Value;

			if (weightSize <= 0)
			{
				DrawDebugBox(GetWorld(), cellLocation, FVector(45.0f, 45.0f, 10.0f), FColor::White, true, -1.0f, 0, 5.0f);
				DrawDebugString(GetWorld(), cellLocation + FVector(0, 0, 20), TEXT("Err"), nullptr, FColor::White, -1.0f, false, 1.0f);
				continue;
			}

			FLinearColor startColor = FLinearColor::Blue;
			FLinearColor endColor = FLinearColor::Red;
			FLinearColor debugColor = FLinearColor::LerpUsingHSV(startColor, endColor, (float)weight / weightSize);

			float baseZ = 10.0f;
			float maxZIncrease = 90.0f;
			float boxHeight = baseZ + ((float)weight / weightSize) * maxZIncrease;
			FVector boxExtent = FVector(45.0f, 45.0f, boxHeight / 2.0f);

			FVector debugLocation = cellLocation + FVector(0, 0, boxExtent.Z);

			float duration = -1.0f;
			float thickness = 5.0f;

			DrawDebugBox(
				GetWorld(),
				debugLocation,
				boxExtent,
				debugColor.ToFColor(true),
				true,
				duration,
				0,
				thickness
			);

			FString debugText = FString::Printf(TEXT("%d"), weight);
			DrawDebugString(
				GetWorld(),
				debugLocation + FVector(0, 0, boxExtent.Z + 10),
				debugText,
				nullptr,
				debugColor.ToFColor(true),
				duration,
				false,
				1.0f
			);
		}
	}
}

void AAI_WeightMapManager::ShowAttackFromPositionsDebug()
{
	if (!bShowAttackFromPositionsDebug || !BattleManager || !BattleManager->PlayerRef)
	{
		return;
	}

	// Iterate through the new map
	for (const TPair<FIntPoint, FActionDescriptionArray>& Entry : possibleAttackTargetActions)
	{
		FIntPoint attackFromPos = Entry.Key;
		const TArray<FString>& actionDescriptions = Entry.Value.Descriptions;

		TObjectPtr<ABattleGridCell>** cellPtr = cellMap.Find(attackFromPos);

		if (cellPtr && *cellPtr)
		{
			FVector cellWorldLocation = (**cellPtr)->GetActorLocation();

			// Draw a debug sphere at this "attack-from" position
			DrawDebugSphere(
				GetWorld(),
				cellWorldLocation + FVector(0, 0, 75.0f), // Slightly higher than arrows
				30.0f, // Radius
				12, // Segments
				FColor::Green, // Color
				true, // Persistent
				-1.0f, // Infinite duration
				0, // Depth priority
				5.0f // Thickness
			);

			// Concatenate action descriptions for display
			FString combinedActionText;
			for (const FString& desc : actionDescriptions)
			{
				if (!combinedActionText.IsEmpty())
				{
					combinedActionText += TEXT(", ");
				}
				combinedActionText += desc;
			}

			// Draw the combined action text above the sphere
			DrawDebugString(
				GetWorld(),
				cellWorldLocation + FVector(0, 0, 120.0f), // Increased Z offset
				combinedActionText,
				nullptr,
				FColor::White, // Changed to White
				-1.0f,
				false,
				1.5f // Increased font scale
			);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Skipping attack-from sphere: Cell (%d,%d) not found in cellMap."), attackFromPos.X, attackFromPos.Y));
		}
	}
}


// Called every frame
void AAI_WeightMapManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (BattleManager && BattleManager->PlayerRef)
	{
		FIntPoint currentPlayerPosition = BattleManager->PlayerRef->GetPos();
		if (currentPlayerPosition != LastPlayerPosition)
		{
			LastPlayerPosition = currentPlayerPosition;
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Magenta, FString::Printf(TEXT("Player moved to: (%d, %d). Updating Weight Map."), currentPlayerPosition.X, currentPlayerPosition.Y));
			GetCurrentActionPositionsForPlayer(); // This will regenerate and show the weight map
		}
	}
}
