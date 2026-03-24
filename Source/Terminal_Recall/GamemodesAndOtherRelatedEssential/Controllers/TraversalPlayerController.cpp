#include "TraversalPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/EntityClasses/WorldTraversingEntityV2.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/DefaultInstance.h"
#include "Terminal_Recall/GridCellClasses/HarryWorldTraversalNode.h"
#include "Terminal_Recall/GridCellClasses/NodeInfoForCellSearch.h"
#include "Terminal_Recall/StuffForUI/LevelSelectWindow.h"

void ATraversalPlayerController::OpenLevelSelect(int32 difficulty, const TArray<int32>& enemiesID)
{
	LevelSelectWindow->AddToViewport();
	LevelSelectWindow->PassInfoToLevelSelectWindow(difficulty, enemiesID);
	ScreenOn = true;
}

void ATraversalPlayerController::NavigateWorld(TObjectPtr<AHarryWorldTraversalNode> cell)
{
	if (DestinationNode == cell)
	{
		UE_LOG(LogTemp, Warning, TEXT("Same dest"));
	}
	if (ScreenOn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Screen on"));
		return;
	}
	if (cell == CurrentNode)
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
		while (IsValid(current))
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
	UE_LOG(LogTemp, Warning, TEXT("residing cell = %s"), *cell->GetActorNameOrLabel());
}

void ATraversalPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &ATraversalPlayerController::DelayedBeginPlay, 1.0f, false);
}

void ATraversalPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (MappingContext)
			subsystem->AddMappingContext(MappingContext, 0);
	}
}

void ATraversalPlayerController::NavigateOneSpaceBack()
{
	Algo::Reverse(ThePath);
	ThePath.SetNum(2, EAllowShrinking::Yes);
	ScreenOn = false;
	DestinationNode = nullptr;
	PlayerEntity->StartMove(ThePath);
}

bool ATraversalPlayerController::FindCell(TObjectPtr<UNodeInfoForCellSearch> currentInfo)
{
	// 
	TObjectPtr<UNodeInfoForCellSearch> cellInfo = currentInfo;
	int32 iterations = 0;
	while (OpenList.Num() > 0)
	{
		iterations++;
		if (iterations > 150)
			return false;
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
			
			for (const auto& info : CloseList)
			{
				if (info->Node == neighbour.Node)
				{
					found = true;
					break;
				}
			}
			if (found)
				break;
			
			for (const auto& info : OpenList)
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

void ATraversalPlayerController::DelayedBeginPlay()
{
	LevelSelectWindow = Cast<ULevelSelectWindow>(CreateWidget(GetWorld()->GetFirstPlayerController(), LevelSelectUIClass));
	// (gets game instance because it will have the player spawn point)
	UDefaultInstance* gameInstance =  Cast<UDefaultInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (IsValid(gameInstance) && IsValid(PlayerChar))
	{
		FVector spawnPos =  gameInstance->PlayerSpawnPos + FVector(0.0,0.0,100.0f);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FRotator Rotation(0,0,0);
		PlayerEntity = GetWorld()->SpawnActor<AWorldTraversingEntityV2>(PlayerChar, spawnPos, Rotation, SpawnParams);
	}
	// raycast to get the cell that player spawn on
	FVector endTracePos = PlayerEntity->GetActorLocation() + FVector(0,0, -1000);
	FCollisionQueryParams collisionParams;
	FHitResult hitResult;
	GetWorld()->LineTraceSingleByChannel(hitResult, PlayerEntity->GetActorLocation(), endTracePos, ECC_GameTraceChannel1, collisionParams);
	if (hitResult.GetActor())
	{
		CurrentNode = Cast<AHarryWorldTraversalNode>(hitResult.GetActor());
		CurrentNode->OnUnitArrive_Implementation();
	}
	for (const auto& pos : gameInstance->VisitedCells)
	{
		FVector rayStart = pos + FVector(0.0,0.0,100.0f);
		endTracePos = rayStart + FVector(0.0,0.0,-200.0f);
		GetWorld()->LineTraceSingleByChannel(hitResult, rayStart, endTracePos, ECC_GameTraceChannel1, collisionParams);
		if (hitResult.GetActor())
		{
			if (AHarryWorldTraversalNode* foundNode = Cast<AHarryWorldTraversalNode>(hitResult.GetActor()))
			{
				foundNode->OnUnitArrive_Implementation();
			}
		}
	}
}
