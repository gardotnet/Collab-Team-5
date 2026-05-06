#include "HarryWorldTraversalNode.h"

#include "HarryWorldTraversalSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/ForWorldTraversal/PathActor.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/DefaultInstance.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/Controllers/TraversalPlayerController.h"
#include "Terminal_Recall/Harry/TraversalFunctionLibrary.h"

AHarryWorldTraversalNode::AHarryWorldTraversalNode()
{
	PrimaryActorTick.bCanEverTick = false;
	#if WITH_EDITOR
	bRunConstructionScriptOnDrag = false;
	#endif
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
	if (!IsCellHidden)
		Clickable = true;
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

	if (!IsOutpost) return;
	
	if (!IsValid(GameInstance)) {
		GameInstance = Cast<UDefaultInstance>(GetGameInstance());
		if (!GameInstance) return;
	}

	// OUTPOST GAME END CUTSCENE TRIGGERING
	if (IsOutpost) {
		if (GameInstance->HasFixedMech() && !GameInstance->bGameEndEligible) {
			GameInstance->bGameEndEligible = true;
			GameInstance->TriggerGameEnd();
		}
	}

	// NODE VISITING
	if (GameInstance->VisitedCells.IsEmpty())
		return;
	if (GameInstance->VisitedCells.Contains(GetActorLocation()))
		Visited = true;
	else
	{
		if (IsEncounter)
		{
			GameInstance->Map = LevelInfo.LevelFilePath;
			GameInstance->PlayerSpawnPos = GetActorLocation();
			GameInstance->RobotPickUpClass = LevelInfo.RobotPickupBP;
			GameInstance->PickUpLoc = LevelInfo.PartSpawnLoc;
			PlayerController->OpenLevelSelect(LevelInfo.Difficulty, LevelInfo.EnemyIdForHints);
		}
		else
		{
			Visited = true;
		}
	}
	
	if (!Visited)
		return;
	if (NeighbourCloudRemoved)
		return;
	
	Clickable = true;
	for (auto& neighbour : NeighbouringNodes)
	{
		if (!IsValid(neighbour.Node))
			continue;
		if (IsValid(neighbour.Node->CoverObject))
			neighbour.Node->CoverObject->Destroy();
	}
	NeighbourCloudRemoved = true;
}

void AHarryWorldTraversalNode::CreateSinglePath(FHarryNeighbourStruct* neighbour)
{
	this->Modify();
	
	if (neighbour->HasPathConnected)
		return;
	neighbour->HasPathConnected = true;
	if (!IsValid(WorldTraversalSystem))
		WorldTraversalSystem = Cast<AHarryWorldTraversalSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), AHarryWorldTraversalSystem::StaticClass()));
	// finding itself to see if it is in its neighbour's list
	FHarryNeighbourStruct* foundSelf = neighbour->Node->NeighbouringNodes.FindByPredicate([this](const FHarryNeighbourStruct& item)
	{
		return item.Node == this;
	});
	
	if (!foundSelf)
	{
		FHarryNeighbourStruct NewStruct;
		NewStruct.Node = this;
		neighbour->Node->NeighbouringNodes.Add(NewStruct);

		foundSelf = neighbour->Node->NeighbouringNodes.FindByPredicate([this](const FHarryNeighbourStruct& item)
		{
			return item.Node == this;
		});
	}

	if (foundSelf)
	{
		foundSelf->HasPathConnected = true;
	}

	// Calculate the position of the path (spline)
	FVector PosA = GetActorLocation();
	FVector PosB = neighbour->Node->GetActorLocation();
	PosB.Z = PosA.Z; // keep on a 2D plane (same height)
	
	float Dist = FVector::Dist(PosA, PosB);
	
	FVector SpawnPoint = (PosA + PosB) * 0.5f;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (APathActor* NewPath = GetWorld()->SpawnActor<APathActor>(WorldTraversalSystem->SplinePathBpClass, SpawnPoint, FRotator::ZeroRotator, SpawnParams))
	{
		NewPath->Tags.Add(FName("TraversalPathActor"));
		NewPath->LinkStruct(WorldTraversalSystem, this, *neighbour);
		UCustomSplineComp* SplineComp = Cast<UCustomSplineComp>(NewPath->GetComponentByClass(UCustomSplineComp::StaticClass()));
		if (!IsValid(SplineComp))
		{
			SplineComp = NewObject<UCustomSplineComp>(NewPath, UCustomSplineComp::StaticClass(), TEXT("PathSpline"));
			if (IsValid(SplineComp))
			{
				SplineComp->RegisterComponent();
				NewPath->AddInstanceComponent(SplineComp);
			}
		}

		if (IsValid(SplineComp))
		{
			SplineComp->ClearSplinePoints(true);
			SplineComp->AddSplinePoint(PosA, ESplineCoordinateSpace::World, true);
			SplineComp->AddSplinePoint(PosB, ESplineCoordinateSpace::World, true);
		}

		neighbour->PathObject = NewPath;
		neighbour->PathSplineComponent = SplineComp;
		neighbour->Length = Dist / 100;

		if (foundSelf)
		{
			foundSelf->PathObject = NewPath;
			foundSelf->PathSplineComponent = SplineComp;
			foundSelf->Length = Dist / 100;
		}
		
		WorldTraversalSystem->RebuildMeshesForPath(neighbour);
	}
}

#if WITH_EDITOR
void AHarryWorldTraversalNode::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (!VariableLoaded)
	{
		VariableLoaded = true;
		FTimerHandle timerHandle;
		GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &AHarryWorldTraversalNode::InitializeVariables, 1.0f, false);
	}
	else
	{
		for (auto neighbour : NeighbouringNodes)
		{
			if (!IsValid(neighbour.PathSplineComponent))
				continue;
			UCustomSplineComp* SplineComp = neighbour.PathSplineComponent;
			FVector startPos;
			FRotator startRot;
			UTraversalFunctionLibrary::EvaluateSplineMovement(SplineComp, 0, false, startPos, startRot );
			if (startPos == neighbour.Node->GetActorLocation())
			{
				SplineComp->SetLocationAtSplinePoint(0, GetActorLocation(), ESplineCoordinateSpace::World, true);
			}
			else
			{
				
				SplineComp->SetLocationAtSplinePoint(SplineComp->GetNumberOfSplinePoints() - 1, GetActorLocation(), ESplineCoordinateSpace::World, true);
			}
			if (!IsValid(WorldTraversalSystem))
				WorldTraversalSystem = Cast<AHarryWorldTraversalSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), AHarryWorldTraversalSystem::StaticClass()));
			WorldTraversalSystem->RebuildMeshesForPath(&neighbour);
		}
	}
}

void AHarryWorldTraversalNode::PostActorCreated()
{
	Super::PostActorCreated();
	VariableLoaded = true;
	bRunConstructionScriptOnDrag = true;
	PostEditChange();
}

void AHarryWorldTraversalNode::InitializeVariables()
{
	OldNeighbouringNodes.Empty();
	OldNeighbouringNodes.Append(NeighbouringNodes);
	bRunConstructionScriptOnDrag = true;
	PostEditChange();
}

void AHarryWorldTraversalNode::PreEditChange(FProperty* PropertyAboutToChange)
{
	this->Modify();
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
	this->Modify();
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
                        oldNeighbourRef.Node->Modify();
                        oldNeighbourRef.Node->NeighbouringNodes.RemoveAt(indexFoundAt);

                    	(void) oldNeighbourRef.Node->MarkPackageDirty(); 
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
    				oldNeighbourRef.Node->Modify();
    				oldNeighbourRef.Node->NeighbouringNodes.RemoveAt(indexFoundAt);

    				FProperty* Prop = oldNeighbourRef.Node->GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(AHarryWorldTraversalNode, NeighbouringNodes));
    				FPropertyChangedEvent ChildChangeEvent(Prop, EPropertyChangeType::ArrayRemove);
    				oldNeighbourRef.Node->PostEditChangeProperty(ChildChangeEvent);
    			}
    		}
    	}
        else if (NeighbouringNodes.IsValidIndex(index))
        {
        	FName propertyName = (PropertyChangedEvent.Property != nullptr) 
				? PropertyChangedEvent.Property->GetFName() : NAME_None;
        	FHarryNeighbourStruct* neighbourInfo = &NeighbouringNodes[index];
        	AHarryWorldTraversalNode* refToLastNeighbour = NeighbouringNodes[index].Node;
        	if (!IsValid(refToLastNeighbour))
        	{
        		if (!OldNeighbouringNodes.IsValidIndex(index) || !IsValid(OldNeighbouringNodes[index].Node))
        			return;
        		refToLastNeighbour = OldNeighbouringNodes[index].Node;
        	}
        	if (this == neighbourInfo->Node)
        	{
        		
        		neighbourInfo->PathObject->Destroy();
        		NeighbouringNodes.RemoveAt(index);
        		return;
        	}
			// if what was changed was the cell/ref to other neighbour
        	if (propertyName == FName("Node"))
        	{
        		// if it was previously linked to another cell
        		if (IsValid(neighbourInfo->PathObject))
        		{
        			// remove the path
        			neighbourInfo->PathObject->Destroy();
        			neighbourInfo->PathObject = nullptr;
        			neighbourInfo->PathSplineComponent = nullptr;
        			neighbourInfo->HasPathConnected = false;
        			int32 indexFoundAt = refToLastNeighbour->NeighbouringNodes.IndexOfByPredicate([this](const FHarryNeighbourStruct& Item) {
						return Item.Node == this;
					});
        			// remove itself from disconnected neighbour's list
        			if (indexFoundAt != INDEX_NONE)
        			{
        				refToLastNeighbour->Modify();
        				refToLastNeighbour->NeighbouringNodes.RemoveAt(indexFoundAt);

        				FProperty* Prop = refToLastNeighbour->GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(AHarryWorldTraversalNode, NeighbouringNodes));
        				FPropertyChangedEvent ChildChangeEvent(Prop, EPropertyChangeType::ArrayRemove);
        				refToLastNeighbour->PostEditChangeProperty(ChildChangeEvent);
        			}
        		}
        		// if chosen a valid ref to another cell 
        		if (IsValid(neighbourInfo->Node))
        		{
        			if (OldNeighbouringNodes.IsValidIndex(index))
        			{
        				if (OldNeighbouringNodes[index].Node == neighbourInfo->Node)
        					return;
        			}        		
        			
        			UE_LOG(LogTemp, Warning, TEXT("Im running"));
        			//CreateSinglePath(neighbourInfo);
        			if (!IsValid(WorldTraversalSystem))
        				WorldTraversalSystem = Cast<AHarryWorldTraversalSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), AHarryWorldTraversalSystem::StaticClass()));
        			WorldTraversalSystem->ActualPathCreation(this, neighbourInfo);
        			WorldTraversalSystem->RebuildMeshesForPath(neighbourInfo);
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
	OldNeighbouringNodes.Empty();
	OldNeighbouringNodes.Append(NeighbouringNodes);
}
#endif
