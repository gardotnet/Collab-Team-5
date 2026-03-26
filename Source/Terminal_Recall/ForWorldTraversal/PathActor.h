#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Terminal_Recall/StructsAndEnums/HarryNeighbourStruct.h"
#include "PathActor.generated.h"

class AHarryWorldTraversalSystem;
class USplineComponent;

UCLASS()
class TERMINAL_RECALL_API APathActor : public AActor
{
	GENERATED_BODY()

public:
	APathActor();

protected:
	UPROPERTY()
	AHarryWorldTraversalSystem* WorldTraversalSystem;
	FHarryNeighbourStruct* RelatedStruct;
	
public:
	void LinkStruct(AHarryWorldTraversalSystem* system, FHarryNeighbourStruct* relatedStruct);
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
};
