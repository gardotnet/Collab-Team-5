#include "GridVisualiser.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/TextRenderComponent.h"

AGridVisualiser::AGridVisualiser()
{
	PrimaryActorTick.bCanEverTick = true;
	
	USceneComponent* DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = DummyRoot;
	GridVisualComp = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Grid Visual");
	GridVisualComp->SetupAttachment(DummyRoot);
	SetFlags(RF_Transient);
}

void AGridVisualiser::RegenerateGrid(FIntPoint gridSize)
{
	if (!GridVisualComp || !GridVisualComp->GetStaticMesh())
		return;
	
	auto color = { 1.0f, 1.0f, 1.0f, 1.0f };
	ClearText();
	GridVisualComp->ClearInstances();
	for (int y = 0; y < gridSize.Y; y++)
	{
		for (int x = 0; x < gridSize.X; x++)
		{
			FVector relativeLoc = FVector(y * TileSize, x * TileSize, 0.0f);
			FTransform instanceTransform = FTransform(relativeLoc);
			int32 instanceIndex =  GridVisualComp->AddInstance(instanceTransform);
			GridVisualComp->SetCustomData(instanceIndex, color, false);
		}
	}
}

void AGridVisualiser::AddText(int32 ismIndex, FText text, FIntPoint groupSize)
{
	FTransform childTransform;
	UE_LOG(LogTemp, Warning, TEXT("Group Size: ( %d, %d)"), groupSize.X, groupSize.Y);
	if (GridVisualComp->GetInstanceTransform(ismIndex, childTransform, true))
	{
		FVector newLoc = childTransform.GetLocation() - FVector(50.0f, 50.0f, -0.1f);
		newLoc += FVector( 50.0f * (groupSize.Y - 1),50.0f * (groupSize.X - 1), 0.0f);
		UTextRenderComponent* newTextComp = NewObject<UTextRenderComponent>(this);
		newTextComp->RegisterComponent();
		newTextComp->SetWorldSize(26 * groupSize.Y);
		newTextComp->SetWorldLocation(newLoc);
		newTextComp->SetText(text);
		newTextComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		newTextComp->SetHorizontalAlignment(EHTA_Center);
		newTextComp->VerticalAlignment = EVRTA_TextCenter;
		newTextComp->SetRelativeRotation(FRotator(90.0f, 180.0f, 0.0f)); 
		TextComponentList.Add(ismIndex, *newTextComp);
	}
}

void AGridVisualiser::SetText(int32 ismIndex, FText text, FIntPoint groupSize)
{
	/*if (auto* textComp = TextComponentList.Find(ismIndex))
	{
		(*textComp)->SetText(text);
	}*/
	UE_LOG(LogTemp, Warning, TEXT("Group Size: ( %d, %d)"), groupSize.X, groupSize.Y);
	FTransform childTransform;
	if (GridVisualComp->GetInstanceTransform(ismIndex, childTransform, true))
	{
		FVector newLoc = childTransform.GetLocation() - FVector(50.0f, 50.0f, -0.1f);
		newLoc += FVector( 50.0f * (groupSize.Y - 1),50.0f * (groupSize.X - 1), 0.0f);
		if (auto* textComp = TextComponentList.Find(ismIndex))
		{
			(*textComp)->SetText(text);
			(*textComp)->SetWorldLocation(newLoc);
			(*textComp)->SetWorldSize(26 * groupSize.Y);;
		}
	}
}

void AGridVisualiser::DeleteText(int32 ismIndex)
{
	TObjectPtr<UTextRenderComponent> outComponent;
	if (TextComponentList.RemoveAndCopyValue(ismIndex, outComponent))
	{
		if (outComponent)
		{
			outComponent->DestroyComponent();
		}
	}
}

void AGridVisualiser::ClearText()
{
	for (auto& pair : TextComponentList)
	{
		if (pair.Value)
		{
			pair.Value->DestroyComponent();
		}
	}
	TextComponentList.Empty();
}
