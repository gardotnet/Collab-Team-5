#include "HarryWorldTraversalNode.h"

#include "HarryWorldTraversalSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/DefaultInstance.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/Controllers/TraversalPlayerController.h"

AHarryWorldTraversalNode::AHarryWorldTraversalNode()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AHarryWorldTraversalNode::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<ATraversalPlayerController>(GetWorld()->GetFirstPlayerController());
	GameInstance = Cast<UDefaultInstance>(GetGameInstance());
	if (LevelID > 0)
	{
		FString stringConvert = FString::FromInt(LevelID);
		LevelInfo = *LevelDataTable->FindRow<FLevelInfoStruct>(FName(*stringConvert), TEXT(""));
	}
	WorldTraversalSystem = Cast<AHarryWorldTraversalSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), AHarryWorldTraversalSystem::StaticClass()));
}

void AHarryWorldTraversalNode::OnCellClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	if (IsValid(PlayerController))
	{
		if (Clickable)
		{
			PlayerController->NavigateWorld(this);
		}
	}
}

void AHarryWorldTraversalNode::OnUnitArrive_Implementation()
{
	Super::OnUnitArrive_Implementation();
	if (!IsValid(GameInstance))
		GameInstance = Cast<UDefaultInstance>(GetGameInstance());
	if (GameInstance->VisitedCells.IsEmpty())
		return;
	if (GameInstance->VisitedCells.Contains(GetActorLocation()))
		Visited = true;
	else
	{
		GameInstance->Map = LevelInfo.LevelFilePath;
		GameInstance->PlayerSpawnPos = GetActorLocation();
		PlayerController->OpenLevelSelect(LevelInfo.Difficulty, LevelInfo.EnemyIdForHints);
	}
	
	if (!Visited)
		return;
	if (NeighbourCloudRemoved)
		return;
	
	Clickable = true;
	for (auto& neighbour : NeighbouringNodes)
	{
		if (IsValid(neighbour.Node->CoverObject))
			neighbour.Node->CoverObject->Destroy();
	}
}

void AHarryWorldTraversalNode::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);
	// if the neighbour list is being changed
	if (PropertyAboutToChange && PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(AHarryWorldTraversalNode, NeighbouringNodes))
	{
		// safe previous state so that when the neighbour is removed we still have reference to it
		OldNeighbouringNodes.Empty(); 
		OldNeighbouringNodes.Append(NeighbouringNodes); 
	}
}

void AHarryWorldTraversalNode::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	FEditPropertyChain::TDoubleLinkedListNode* MemberNode = PropertyChangedEvent.PropertyChain.GetActiveMemberNode();
	FName MemberName = (MemberNode != nullptr) ? MemberNode->GetValue()->GetFName() : NAME_None;
	if (MemberName == GET_MEMBER_NAME_CHECKED(AHarryWorldTraversalNode, NeighbouringNodes))
    {
    	// get where change happened
        int32 index = PropertyChangedEvent.GetArrayIndex(MemberName.ToString());
    	// if an index was removed
        if (index == INDEX_NONE || PropertyChangedEvent.ChangeType == EPropertyChangeType::ArrayRemove)
        {
        	// looping through old list to find what neighbour is removed
        	// (can't use index because I get -1 when removing)
            for (auto& oldNeighbourRef : OldNeighbouringNodes)
            {
            	// check if it was the one removed
                bool bStillExists = NeighbouringNodes.ContainsByPredicate([&oldNeighbourRef](const FHarryNeighbourStruct& Current) {
                    return oldNeighbourRef.Node == Current.Node;
                });
            	// if it has been removed
                if (!bStillExists && oldNeighbourRef.Node)
                {
                	// delete path
                    if (oldNeighbourRef.PathObject) { oldNeighbourRef.PathObject->Destroy(); }
                	// remove itself for neighbour's list
                    int32 indexFoundAt = oldNeighbourRef.Node->NeighbouringNodes.IndexOfByPredicate([this](const FHarryNeighbourStruct& Item) {
                        return Item.Node == this;
                    });
                    if (indexFoundAt != INDEX_NONE)
                    {
                        //oldNeighbourRef.Node->Modify();
                        oldNeighbourRef.Node->NeighbouringNodes.RemoveAt(indexFoundAt);
                    }
                }
            }
        }
    	// if neighbours has been cleared
    	else if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ArrayClear)
    	{
    		// same as delete but this time no need to search, just clear all neighbours
    		for (auto& oldNeighbourRef : NeighbouringNodes)
    		{
    			if (oldNeighbourRef.PathObject) { oldNeighbourRef.PathObject->Destroy(); }

    			int32 indexFoundAt = oldNeighbourRef.Node->NeighbouringNodes.IndexOfByPredicate([this](const FHarryNeighbourStruct& Item) {
					return Item.Node == this;
				});

    			if (indexFoundAt != INDEX_NONE)
    			{
    				//oldNeighbourRef.Node->Modify();
    				oldNeighbourRef.Node->NeighbouringNodes.RemoveAt(indexFoundAt);
    			}
    		}
    	}
        else if (NeighbouringNodes.IsValidIndex(index))
        {
        	FName propertyName = (PropertyChangedEvent.Property != nullptr) 
				? PropertyChangedEvent.Property->GetFName() : NAME_None;
        	FHarryNeighbourStruct* neighbour = &NeighbouringNodes[index];
			// if what was changed was the cell/ref to other neighbour
        	if (propertyName == FName("Cell"))
        	{
        		// if it was previously linked to another cell
        		if (neighbour->PathObject != nullptr)
        		{
        			// remove the path
        			neighbour->PathObject->Destroy();
        			neighbour->PathObject = nullptr;
        			neighbour->HasPathConnected = false;
        			int32 indexFoundAt = neighbour->Node->NeighbouringNodes.IndexOfByPredicate([this](const FHarryNeighbourStruct& Item) {
						return Item.Node == this;
					});
        			// remove itself from disconnected neighbour's list
        			if (indexFoundAt != INDEX_NONE)
        			{
        				neighbour->Node->Modify();
        				neighbour->Node->NeighbouringNodes.RemoveAt(indexFoundAt);
        			}
        		}
        		// if chosen a valid ref to another cell 
        		if (neighbour->Node != nullptr)
        		{
        			WorldTraversalSystem->CreatePaths();
        		}
        	}
        }
    }
	else if (MemberName == GET_MEMBER_NAME_CHECKED(AHarryWorldTraversalNode, IsCellHidden))
	{
		// if toggle on then 
		if (IsCellHidden)
		{
			// create new hider
			FActorSpawnParameters spawnParams;
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			FVector spawnLocation = GetActorLocation() + CoverOffset;
			spawnLocation.Z = CoverHeight;
			CoverObject = GetWorld()->SpawnActor<AActor>(CellCoverBpClass, spawnLocation, FRotator(), spawnParams);
			CoverObject->SetActorRelativeScale3D(CoverSize);
			// need to set material because when spawned by post edit it use default material 
			if (UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(CoverObject->GetComponentByClass(UStaticMeshComponent::StaticClass())))
			{
				if (CoverMaterial)
					MeshComp->SetMaterial(0, CoverMaterial);
			}
		}
		else
		{
			// remove the hider
			if (IsValid(CoverObject))
			{
				CoverObject->Destroy();
				CoverObject = nullptr;
			}
		}
	}
	else if (MemberName == GET_MEMBER_NAME_CHECKED(AHarryWorldTraversalNode, CoverSize))
	{
		if (IsValid(CoverObject))
		{
			CoverObject->SetActorRelativeScale3D(CoverSize);
		}
	}
	else if (MemberName == GET_MEMBER_NAME_CHECKED(AHarryWorldTraversalNode, CoverHeight) ||
		MemberName == GET_MEMBER_NAME_CHECKED(AHarryWorldTraversalNode, CoverOffset))
	{
		FVector coverLoc = GetActorLocation() + CoverOffset;
		coverLoc.Z = CoverHeight;
		CoverObject->SetActorLocation(coverLoc);
	}
}

