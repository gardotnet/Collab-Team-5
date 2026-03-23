#include "WorldGridCell.h"

#include "Kismet/KismetMathLibrary.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/Controllers/DefaultPlayerController.h"

AWorldGridCell::AWorldGridCell()
{
	PrimaryActorTick.bCanEverTick = true;
}
void AWorldGridCell::LoadLevel()
{
	// [NOTE] Will update game mode and to load the level assign to this cell.
}
void AWorldGridCell::CreatePath()
{
	for (auto& neighbour : NeighbouringCells)
	{
		if (neighbour.Cell != nullptr)
			ActualPathCreation(&neighbour);
	}
}
void AWorldGridCell::ResetPathPosition()
{
	for (auto& neighbour : NeighbouringCells)
	{
		if (neighbour.Cell == nullptr)
			continue;		
		
		// Calculate the position, rotation and scale of the path
		FVector currentPos = GetActorLocation();
		FVector neighbourPos = neighbour.Cell->GetActorLocation();
		neighbourPos.Z = currentPos.Z;
		float dist = FVector::Dist(currentPos, neighbourPos);
		// half-way between 2 points
		FVector midPoint = (currentPos + neighbourPos) * 0.5f;
		FVector scale = FVector(dist/100, 0.9f , 1);
		FRotator rotation = UKismetMathLibrary::FindLookAtRotation(midPoint, neighbourPos);
		
		if (neighbour.PathObject != nullptr)
		{
			neighbour.PathObject->SetActorLocation(midPoint);
			neighbour.PathObject->SetActorRotation(rotation);
			neighbour.PathObject->SetActorRelativeScale3D(scale);
		}
		else
		{
			// create a path if the path is missing
			FActorSpawnParameters spawnParams;
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AActor* newPath = GetWorld()->SpawnActor<AActor>(PathBpClass, midPoint, rotation, spawnParams);
			newPath->SetActorScale3D(scale);
		}
	}
}
void AWorldGridCell::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<ADefaultPlayerController>(GetWorld()->GetFirstPlayerController());
}
void AWorldGridCell::OnCellClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	if (PlayerController)
	{
		if (Clickable)
		{
			PlayerController->NavigateWorld(this);
		}
	}
}
void AWorldGridCell::OnUnitArrive_Implementation()
{
	Super::OnUnitArrive_Implementation();
	if (!Visited)
	{
		Visited = true;
		for (auto& neighbour : NeighbouringCells)
		{
			if (neighbour.Cell->CoverObject)
				neighbour.Cell->CoverObject->Destroy();
		}
	}
}
void AWorldGridCell::ActualPathCreation(FWorldNeighbourStruct* neighbour)
{
	if (neighbour->HasPathConnected) // Checks if the chosen neighbour has already been connected, if so, return
			return;
	neighbour->HasPathConnected = true; // sets the connected path status to true if not already listed as connected
	
	// finding itself to see if it is in its neighbour's list
	
	FWorldNeighbourStruct* foundSelf = neighbour->Cell->NeighbouringCells.FindByPredicate([this](const FWorldNeighbourStruct& item)
	{
		return item.Cell == this;
	});
	
	if (foundSelf)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wow already prepared are we?"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding Path"));
		
		// Calculate the position and rotation of the path
		FVector currentPos = GetActorLocation();
		FVector neighbourPos = neighbour->Cell->GetActorLocation();
		
		neighbourPos.Z = currentPos.Z; // sets same height for current position to neighbour position
		
		float dist = FVector::Dist(currentPos, neighbourPos); // gets the distance between the two points
		
		// half-way between 2 points
		FVector spawnPoint = (currentPos + neighbourPos) * 0.5f;
		FRotator rotation = UKismetMathLibrary::FindLookAtRotation(spawnPoint, neighbourPos);
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AActor* newPath = GetWorld()->SpawnActor<AActor>(PathBpClass, spawnPoint, rotation, spawnParams);
		newPath->SetActorScale3D(FVector(dist/100, 0.9f , 1));
		
		// updating info
		neighbour->PathObject = newPath;
		neighbour->Length = dist/100;
		
		FWorldNeighbourStruct newStruct;
		newStruct.Cell = this;
		newStruct.Length = dist/100;
		newStruct.HasPathConnected = true;
		newStruct.PathObject = newPath;
		neighbour->Cell->NeighbouringCells.Add(newStruct);
	}
}
#if WITH_EDITOR
void AWorldGridCell::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);
	// if the neighbour list is being changed
	if (PropertyAboutToChange && PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(AWorldGridCell, NeighbouringCells))
	{
		// safe previous state so that when the neighbour is removed we still have reference to it
		OldNeighbouringCells.Empty(); 
		OldNeighbouringCells.Append(NeighbouringCells); 
	}
}
void AWorldGridCell::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
    Super::PostEditChangeChainProperty(PropertyChangedEvent);
    FEditPropertyChain::TDoubleLinkedListNode* MemberNode = PropertyChangedEvent.PropertyChain.GetActiveMemberNode();
    FName MemberName = (MemberNode != nullptr) ? MemberNode->GetValue()->GetFName() : NAME_None;
	// if the neighbour list is being changed
    if (MemberName == GET_MEMBER_NAME_CHECKED(AWorldGridCell, NeighbouringCells))
    {
    	// get where change happened
        int32 index = PropertyChangedEvent.GetArrayIndex(MemberName.ToString());
    	// if an index was removed
        if (index == INDEX_NONE || PropertyChangedEvent.ChangeType == EPropertyChangeType::ArrayRemove)
        {
        	// looping through old list to find what neighbour is removed
        	// (can't use index because I get -1 when removing)
            for (auto& oldNeighbourRef : OldNeighbouringCells)
            {
            	// check if it was the one removed
                bool bStillExists = NeighbouringCells.ContainsByPredicate([&oldNeighbourRef](const FWorldNeighbourStruct& Current) {
                    return oldNeighbourRef.Cell == Current.Cell;
                });
            	// if it has been removed
                if (!bStillExists && oldNeighbourRef.Cell)
                {
                	// delete path
                    if (oldNeighbourRef.PathObject) { oldNeighbourRef.PathObject->Destroy(); }
                	// remove itself for neighbour's list
                    int32 indexFoundAt = oldNeighbourRef.Cell->NeighbouringCells.IndexOfByPredicate([this](const FWorldNeighbourStruct& Item) {
                        return Item.Cell == this;
                    });
                    if (indexFoundAt != INDEX_NONE)
                    {
                        oldNeighbourRef.Cell->Modify();
                        oldNeighbourRef.Cell->NeighbouringCells.RemoveAt(indexFoundAt);
                    }
                }
            }
        }
    	// if neighbours has been cleared
    	else if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ArrayClear)
    	{
    		// same as delete but this time no need to search, just clear all neighbours
    		for (auto& oldNeighbourRef : OldNeighbouringCells)
    		{
    			if (oldNeighbourRef.PathObject) { oldNeighbourRef.PathObject->Destroy(); }

    			int32 indexFoundAt = oldNeighbourRef.Cell->NeighbouringCells.IndexOfByPredicate([this](const FWorldNeighbourStruct& Item) {
					return Item.Cell == this;
				});

    			if (indexFoundAt != INDEX_NONE)
    			{
    				oldNeighbourRef.Cell->Modify();
    				oldNeighbourRef.Cell->NeighbouringCells.RemoveAt(indexFoundAt);
    			}
    		}
    	}
        else if (NeighbouringCells.IsValidIndex(index))
        {
        	FName propertyName = (PropertyChangedEvent.Property != nullptr) 
				? PropertyChangedEvent.Property->GetFName() : NAME_None;
        	FWorldNeighbourStruct* neighbour = &NeighbouringCells[index];
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
        			int32 indexFoundAt = neighbour->Cell->NeighbouringCells.IndexOfByPredicate([this](const FWorldNeighbourStruct& Item) {
						return Item.Cell == this;
					});
        			// remove itself from disconnected neighbour's list
        			if (indexFoundAt != INDEX_NONE)
        			{
        				neighbour->Cell->Modify();
        				neighbour->Cell->NeighbouringCells.RemoveAt(indexFoundAt);
        			}
        		}
        		// if chosen a valid ref to another cell 
        		if (neighbour->Cell != nullptr)
        		{
        			ActualPathCreation(neighbour);
        		}
        	}
        }
    }
	// if toggle hider
	else if (MemberName == GET_MEMBER_NAME_CHECKED(AWorldGridCell, IsCellHidden))
	{
		// if toggle on then 
		if (IsCellHidden)
		{
			// create new hider
			FActorSpawnParameters spawnParams;
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			FVector spawnLocation = GetActorLocation() + CoverOffset;
			spawnLocation.Z = CoverHeight;
			CoverObject = GetWorld()->SpawnActor<AActor>(PathBpClass, spawnLocation, FRotator(), spawnParams);
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
			if (CoverObject != nullptr)
			{
				CoverObject->Destroy();
				CoverObject = nullptr;
			}
		}
	}
	else if (MemberName == GET_MEMBER_NAME_CHECKED(AWorldGridCell, CoverSize))
	{
		if (CoverObject != nullptr)
		{
			CoverObject->SetActorRelativeScale3D(CoverSize);
		}
	}
	else if (MemberName == GET_MEMBER_NAME_CHECKED(AWorldGridCell, CoverHeight) ||
		MemberName == GET_MEMBER_NAME_CHECKED(AWorldGridCell, CoverOffset))
	{
		FVector coverLoc = GetActorLocation() + CoverOffset;
		coverLoc.Z = CoverHeight;
		CoverObject->SetActorLocation(coverLoc);
	}
}
#endif