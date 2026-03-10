#include "DefaultPlayerController.h"
#include "Algo/Reverse.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/GameStates/DefaultGameState.h"
#include "Terminal_Recall/GridCellClasses/NodeInfoForCellSearch.h"
#include "Terminal_Recall/GridCellClasses/WorldGridCell.h"

void ADefaultPlayerController::BeginPlay()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	// (gets game state because it will have the player spawn point)
	ADefaultGameState* gameState = GetWorld()->GetGameState<ADefaultGameState>();
	if (gameState && PlayerChar)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FRotator Rotation(0,0,0);
		PlayerEntity = GetWorld()->SpawnActor<ABaseEntity>(PlayerChar, gameState->PlayerSpawnPos, Rotation, SpawnParams);
		PlayerAIController = PlayerEntity->GetController<AAIController>();
	}
	
	FVector endTracePos = PlayerEntity->GetActorLocation() + FVector(0,0, -1000);
	FCollisionQueryParams collisionParams;
	FHitResult hitResult;
	GetWorld()->LineTraceSingleByChannel(hitResult, PlayerEntity->GetActorLocation(), endTracePos, ECC_GameTraceChannel1, collisionParams);

	if (hitResult.GetActor())
	{
		CurrentCell = Cast<AWorldGridCell>(hitResult.GetActor());
	}
	MoveTimerHandle = FTimerHandle();
}

void ADefaultPlayerController::MovePlayer()
{
	FVector playerPos = PlayerEntity->GetActorLocation();
	FVector cellPos = ThePath[PathIndex]->GetActorLocation();
	float distance = FMath::Sqrt(FMath::Pow(cellPos.X - playerPos.X, 2) + FMath::Pow(cellPos.Y - playerPos.Y, 2));
	UE_LOG(LogTemp, Warning, TEXT("%f"), distance);
	if (distance < 0.5f)
	{
		if (ThePath[PathIndex] == DestinationCell)
		{
			UE_LOG(LogTemp, Warning, TEXT("EndMovement"));
			GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);
		}
		else
		{
			PathIndex++;
			UE_LOG(LogTemp, Warning, TEXT("Next node is %s"), *ThePath[PathIndex]->GetActorLabel());
		}
	}
	else
	{
		cellPos.Z = playerPos.Z;
		PlayerEntity->SetActorLocation(FMath::Lerp(playerPos, cellPos, 0.2f));
	}
}

void ADefaultPlayerController::NavigateWorld(TObjectPtr<AWorldGridCell> cell)
{
	if (DestinationCell == cell)
		return;
	if (cell == CurrentCell)
	{
		UE_LOG(LogTemp, Warning, TEXT("IDIOT, You are standing on it"));
		return;
	}
	DestinationCell = cell;
	MoveTimerHandle.Invalidate();
	PathIndex = 0;
	ThePath.Empty();
	OpenList.Empty();
	CloseList.Empty();
	
	TObjectPtr<UNodeInfoForCellSearch> newInfo = NewObject<UNodeInfoForCellSearch>();
	newInfo->Cell = CurrentCell;
	newInfo->ParentCell = nullptr;
	newInfo->GCost = 0;
	newInfo->HCost = Heuristic_Manhattan(cell->GetActorLocation(), DestinationCell->GetActorLocation());
	OpenList.Add(newInfo);
	
	if (FindCell(newInfo))
	{
		TObjectPtr<UNodeInfoForCellSearch> current = CloseList.Last();
		while (current != nullptr)
		{
			ThePath.Add(current->Cell);
			current = current->ParentCell;
		}
		// Flips the array around
		Algo::Reverse(ThePath);
		// moves the player
		//PlayerAIController->MoveToActor(ThePath[PathIndex]);
		GetWorld()->GetTimerManager().SetTimer(MoveTimerHandle, this, &ADefaultPlayerController::MovePlayer, 0.1f, true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cell Does Not Exist"));
	}
}

// A* algorithm to find the destination
bool ADefaultPlayerController::FindCell(TObjectPtr<UNodeInfoForCellSearch> cellInfo)
{
	TObjectPtr<UNodeInfoForCellSearch> currentInfo = cellInfo;
	while (OpenList.Num() > 0)
	{
		if (currentInfo->Cell == DestinationCell)
		{
			CloseList.Add(currentInfo);
			return true;
		}
		
		for (auto neighbour : currentInfo->Cell->NeighbouringCells)
		{
			float neighbourGCost = currentInfo->GCost + neighbour.Length;
			float distTillDestination = Heuristic_Manhattan(neighbour.Cell->GetActorLocation(), DestinationCell->GetActorLocation());
			
			bool found = false;
			for (auto info : OpenList)
			{
				if (info->Cell == neighbour.Cell)
				{
					if (info->FCost > neighbourGCost + distTillDestination)
					{
						info->UpdateNodeInfo(currentInfo, neighbourGCost, distTillDestination);	
					}
					found = true;
					break;
				}
			}
			
			if (!found)
			{
				TObjectPtr<UNodeInfoForCellSearch> newInfo = NewObject<UNodeInfoForCellSearch>();
				newInfo->Cell = neighbour.Cell;
				newInfo->ParentCell = currentInfo;
				newInfo->GCost = neighbourGCost;
				newInfo->HCost = distTillDestination;
				OpenList.Add(newInfo);
			}
		}
		
		OpenList.Remove(currentInfo);
		CloseList.Add(currentInfo);
		
		if (OpenList.IsEmpty())
			break;
		
		// sorts it smallest to biggest
		Algo::Sort(OpenList, [](const UNodeInfoForCellSearch* A, const UNodeInfoForCellSearch* B)
		{
			return A->FCost < B->FCost; 
		});
		
		currentInfo = OpenList[0];
	}
	return false;
}

float ADefaultPlayerController::Heuristic_Manhattan(FVector start, FVector end)
{
	return FMath::Abs(start.X - end.X) + FMath::Abs(start.Y - end.Y);
}

