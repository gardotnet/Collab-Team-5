// Fill out your copyright notice in the Description page of Project Settings.


#include "TraversalNode.h"
#include "Components/SplineComponent.h"
#include "Terminal_Recall/GridCellClasses/Harry/WorldTraversalNodeSystem.h"
#include "Kismet/GameplayStatics.h"

ATraversalNode::ATraversalNode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATraversalNode::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATraversalNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#if WITH_EDITOR
void ATraversalNode::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);
	if (PropertyAboutToChange && PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(ATraversalNode, NeighbouringNodes))
	{
		OldNeighbouringNodes.Empty(); 
		OldNeighbouringNodes.Append(NeighbouringNodes); 
	}
}

void ATraversalNode::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	FEditPropertyChain::TDoubleLinkedListNode* MemberNode = PropertyChangedEvent.PropertyChain.GetActiveMemberNode();
	FName MemberName = (MemberNode != nullptr) ? MemberNode->GetValue()->GetFName() : NAME_None;

	if (MemberName == GET_MEMBER_NAME_CHECKED(ATraversalNode, NeighbouringNodes))
	{
		int32 Index = PropertyChangedEvent.GetArrayIndex(MemberName.ToString());
		
		TArray<AActor*> Systems;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWorldTraversalNodeSystem::StaticClass(), Systems);
		AWorldTraversalNodeSystem* System = nullptr;
		if (Systems.Num() > 0)
		{
			System = Cast<AWorldTraversalNodeSystem>(Systems[0]);
		}

		if (Index == INDEX_NONE || PropertyChangedEvent.ChangeType == EPropertyChangeType::ArrayRemove)
		{
			for (auto& OldNeighbourRef : OldNeighbouringNodes)
			{
				bool bStillExists = NeighbouringNodes.ContainsByPredicate([&OldNeighbourRef](const FWorldNeighbourStruct& Current) {
					return OldNeighbourRef.Node == Current.Node;
				});

				if (!bStillExists && OldNeighbourRef.Node)
				{
					if (OldNeighbourRef.PathObject) { OldNeighbourRef.PathObject->Destroy(); }
					
					int32 IndexFoundAt = OldNeighbourRef.Node->NeighbouringNodes.IndexOfByPredicate([this](const FWorldNeighbourStruct& Item) {
						return Item.Node == this;
					});
					
					if (IndexFoundAt != INDEX_NONE)
					{
						OldNeighbourRef.Node->Modify();
						OldNeighbourRef.Node->NeighbouringNodes.RemoveAt(IndexFoundAt);
					}
				}
			}
		}
		else if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ArrayClear)
		{
			for (auto& OldNeighbourRef : OldNeighbouringNodes)
			{
				if (OldNeighbourRef.PathObject) { OldNeighbourRef.PathObject->Destroy(); }

				int32 IndexFoundAt = OldNeighbourRef.Node->NeighbouringNodes.IndexOfByPredicate([this](const FWorldNeighbourStruct& Item) {
					return Item.Node == this;
				});

				if (IndexFoundAt != INDEX_NONE)
				{
					OldNeighbourRef.Node->Modify();
					OldNeighbourRef.Node->NeighbouringNodes.RemoveAt(IndexFoundAt);
				}
			}
		}
		else if (NeighbouringNodes.IsValidIndex(Index))
		{
			FName PropertyName = (PropertyChangedEvent.Property != nullptr) 
				? PropertyChangedEvent.Property->GetFName() : NAME_None;
			FWorldNeighbourStruct* Neighbour = &NeighbouringNodes[Index];

			if (PropertyName == FName("Node"))
			{
				if (Neighbour->PathObject != nullptr)
				{
					Neighbour->PathObject->Destroy();
					Neighbour->PathObject = nullptr;
					Neighbour->PathSplineComponent = nullptr;
					Neighbour->HasPathConnected = false;
					
					if (Neighbour->Node)
					{
						int32 IndexFoundAt = Neighbour->Node->NeighbouringNodes.IndexOfByPredicate([this](const FWorldNeighbourStruct& Item) {
							return Item.Node == this;
						});
						
						if (IndexFoundAt != INDEX_NONE)
						{
							Neighbour->Node->Modify();
							Neighbour->Node->NeighbouringNodes.RemoveAt(IndexFoundAt);
						}
					}
				}
				
				if (Neighbour->Node != nullptr && System)
				{
					System->CreatePaths();
				}
			}
		}
	}
}
#endif