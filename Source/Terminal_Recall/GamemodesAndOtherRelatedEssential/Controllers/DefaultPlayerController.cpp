#include "DefaultPlayerController.h"
#include "Algo/Reverse.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/DefaultInstance.h"
#include "Terminal_Recall/GridCellClasses/NodeInfoForCellSearch.h"
#include "Terminal_Recall/GridCellClasses/Harry/Nodes/TraversalNode.h"
#include "Terminal_Recall/EntityClasses/BaseEntity.h"

void ADefaultPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("ADefaultPlayerController::BeginPlay"));
	// (gets game instance because it will have the player spawn point)
	UDefaultInstance* gameInstance =  Cast<UDefaultInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (gameInstance && PlayerChar)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FRotator Rotation(0,0,0);
		PlayerEntity = GetWorld()->SpawnActor<ABaseEntity>(PlayerChar, gameInstance->PlayerSpawnPos, Rotation, SpawnParams);
	}
	// raycast to get the cell that player spawn on
	if (PlayerEntity)
	{
		FVector endTracePos = PlayerEntity->GetActorLocation() + FVector(0,0, -1000);
		FCollisionQueryParams collisionParams;
		FHitResult hitResult;
		GetWorld()->LineTraceSingleByChannel(hitResult, PlayerEntity->GetActorLocation(), endTracePos, ECC_GameTraceChannel1, collisionParams);
		if (hitResult.GetActor())
		{
			CurrentCell = Cast<ATraversalNode>(hitResult.GetActor());
			// CurrentCell->OnUnitArrive(); // ATraversalNode doesn't have OnUnitArrive currently, you can migrate it or handle it.
		}
	}
}

void ADefaultPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (MappingContext)
			subsystem->AddMappingContext(MappingContext, 0);
	}
}

void ADefaultPlayerController::MovePlayer()
{
	CurrentCell = ThePath[PathIndex];
	if (ThePath[PathIndex] == DestinationCell)
	{
		ThePath.Empty();
		// DestinationCell->OnUnitArrive();
	}
	else
	{
		PathIndex++;
		if (PlayerEntity)
		{
			ATraversalNode* NextNode = ThePath[PathIndex];
			
			// Find the spline connection between CurrentCell and NextNode
			USplineComponent* FoundSpline = nullptr;
			bool bMovingForward = true;
			
			// Check if CurrentCell has a path to NextNode
			for (const FWorldNeighbourStruct& Neighbour : CurrentCell->NeighbouringNodes)
			{
				if (Neighbour.Node == NextNode && Neighbour.PathSplineComponent)
				{
					FoundSpline = Neighbour.PathSplineComponent;
					bMovingForward = true;
					break;
				}
			}
			
			// If not found, check if NextNode has a path back to CurrentCell (means we travel backwards on the spline)
			if (!FoundSpline)
			{
				for (const FWorldNeighbourStruct& Neighbour : NextNode->NeighbouringNodes)
				{
					if (Neighbour.Node == CurrentCell && Neighbour.PathSplineComponent)
					{
						FoundSpline = Neighbour.PathSplineComponent;
						bMovingForward = false;
						break;
					}
				}
			}
			
			if (FoundSpline)
			{
				PlayerEntity->StartMoveAlongSpline(FoundSpline, bMovingForward);
			}
			else
			{
				// Fallback to straight-line if spline is missing
				PlayerEntity->StartMove(NextNode->GetActorLocation());
			}
		}
	}
}
void ADefaultPlayerController::NavigateWorld(TObjectPtr<ATraversalNode> cell)
{
	if (DestinationCell == cell)
		return;
	if (cell == CurrentCell && ThePath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("IDIOT, You are standing on it"));
		return;
	}
	DestinationCell = cell;
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
		if (PlayerEntity && ThePath.IsValidIndex(PathIndex + 1))
		{
			// Emulate the logic in MovePlayer to start the first step properly
			ATraversalNode* NextNode = ThePath[PathIndex + 1];
			
			USplineComponent* FoundSpline = nullptr;
			bool bMovingForward = true;
			
			for (const FWorldNeighbourStruct& Neighbour : CurrentCell->NeighbouringNodes)
			{
				if (Neighbour.Node == NextNode && Neighbour.PathSplineComponent)
				{
					FoundSpline = Neighbour.PathSplineComponent;
					bMovingForward = true;
					break;
				}
			}
			
			if (!FoundSpline)
			{
				for (const FWorldNeighbourStruct& Neighbour : NextNode->NeighbouringNodes)
				{
					if (Neighbour.Node == CurrentCell && Neighbour.PathSplineComponent)
					{
						FoundSpline = Neighbour.PathSplineComponent;
						bMovingForward = false;
						break;
					}
				}
			}
			
			PathIndex++; // increment as we are leaving the starting node
			
			if (FoundSpline)
			{
				PlayerEntity->StartMoveAlongSpline(FoundSpline, bMovingForward);
			}
			else
			{
				PlayerEntity->StartMove(NextNode->GetActorLocation());
			}
		}
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
		
		if (currentInfo->Cell)
		{
			for (auto neighbour : currentInfo->Cell->NeighbouringNodes)
			{
				if (!neighbour.Node) continue;
				float neighbourGCost = currentInfo->GCost + neighbour.Length;
				float distTillDestination = Heuristic_Manhattan(neighbour.Node->GetActorLocation(), DestinationCell->GetActorLocation());
				
				bool found = false;
				for (auto info : OpenList)
				{
					if (info->Cell == neighbour.Node)
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
					newInfo->Cell = neighbour.Node;
					newInfo->ParentCell = currentInfo;
					newInfo->GCost = neighbourGCost;
					newInfo->HCost = distTillDestination;
					OpenList.Add(newInfo);
				}
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
