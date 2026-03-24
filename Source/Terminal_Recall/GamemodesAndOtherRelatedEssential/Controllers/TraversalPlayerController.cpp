#include "TraversalPlayerController.h"

#include "Terminal_Recall/EntityClasses/WorldTraversingEntityV2.h"
#include "Terminal_Recall/GridCellClasses/HarryWorldTraversalNode.h"
#include "Terminal_Recall/GridCellClasses/NodeInfoForCellSearch.h"
#include "Terminal_Recall/StuffForUI/LevelSelectWindow.h"

void ATraversalPlayerController::OpenLevelSelect(int32 difficulty, const TArray<int32>& enemiesID)
{
	LevelSelectWindow->AddToViewport();
	LevelSelectWindow->PassInfoToLevelSelectWindow(difficulty, enemiesID);
}

void ATraversalPlayerController::NavigateWorld(TObjectPtr<AHarryWorldTraversalNode> cell)
{
	if (DestinationNode == cell)
		return;
	if (cell == CurrentNode && ThePath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("IDIOT, You are standing on it"));
		return;
	}
	DestinationNode = cell;
	PathIndex = 0;
	ThePath.Empty();
	OpenList.Empty();
	CloseList.Empty();
	TObjectPtr<UNodeInfoForCellSearch> newInfo = NewObject<UNodeInfoForCellSearch>();
	newInfo->Node = CurrentNode;
	newInfo->ParentCell = nullptr;
	newInfo->GCost = 0;
	newInfo->HCost = Heuristic_Manhattan(cell->GetActorLocation(), DestinationNode->GetActorLocation());
	OpenList.Add(newInfo);
	if (FindCell(newInfo))
	{
		TObjectPtr<UNodeInfoForCellSearch> current = CloseList.Last();
		while (current != nullptr)
		{
			ThePath.Add(current->Node);
			current = current->ParentCell;
		}
		// Flips the array around
		Algo::Reverse(ThePath);
		// moves the player
		PlayerEntity->StartMove(ThePath);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cell Does Not Exist"));
	}
}

void ATraversalPlayerController::UpdatePlayerPos(TObjectPtr<AHarryWorldTraversalNode> cell)
{
	CurrentNode = cell;
}

void ATraversalPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ATraversalPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

bool ATraversalPlayerController::FindCell(TObjectPtr<UNodeInfoForCellSearch> currentInfo)
{
	TObjectPtr<UNodeInfoForCellSearch> cellInfo = currentInfo;
	while (OpenList.Num() > 0)
	{
		if (cellInfo->Node == DestinationNode)
		{
			CloseList.Add(cellInfo);
			return true;
		}
		for (auto neighbour : cellInfo->Node->NeighbouringNodes)
		{
			float neighbourGCost = cellInfo->GCost + neighbour.Length;
			float distTillDestination = Heuristic_Manhattan(neighbour.Node->GetActorLocation(), DestinationNode->GetActorLocation());
			
			bool found = false;
			for (auto info : OpenList)
			{
				if (info->Node == neighbour.Node)
				{
					if (info->FCost > neighbourGCost + distTillDestination)
					{
						info->UpdateNodeInfo(cellInfo, neighbourGCost, distTillDestination);	
					}
					found = true;
					break;
				}
			}
			
			if (!found)
			{
				TObjectPtr<UNodeInfoForCellSearch> newInfo = NewObject<UNodeInfoForCellSearch>();
				newInfo->Node = neighbour.Node;
				newInfo->ParentCell = cellInfo;
				newInfo->GCost = neighbourGCost;
				newInfo->HCost = distTillDestination;
				OpenList.Add(newInfo);
			}
		}
		OpenList.Remove(cellInfo);
		CloseList.Add(cellInfo);
		if (OpenList.IsEmpty())
			break;
		// sorts it smallest to biggest
		Algo::Sort(OpenList, [](const UNodeInfoForCellSearch* A, const UNodeInfoForCellSearch* B)
		{
			return A->FCost < B->FCost; 
		});
		cellInfo = OpenList[0];
	}
	return false;
}

float ATraversalPlayerController::Heuristic_Manhattan(FVector start, FVector end)
{
	return FMath::Abs(start.X - end.X) + FMath::Abs(start.Y - end.Y);
}
