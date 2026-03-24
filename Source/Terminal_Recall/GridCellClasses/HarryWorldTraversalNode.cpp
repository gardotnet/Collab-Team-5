#include "HarryWorldTraversalNode.h"

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
	
	for (auto& neighbour : NeighbouringNodes)
	{
		if (IsValid(neighbour.Node->CoverObject))
			neighbour.Node->CoverObject->Destroy();
	}
}

void AHarryWorldTraversalNode::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	FEditPropertyChain::TDoubleLinkedListNode* MemberNode = PropertyChangedEvent.PropertyChain.GetActiveMemberNode();
	FName MemberName = (MemberNode != nullptr) ? MemberNode->GetValue()->GetFName() : NAME_None;
	if (MemberName == GET_MEMBER_NAME_CHECKED(AHarryWorldTraversalNode, IsCellHidden))
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

