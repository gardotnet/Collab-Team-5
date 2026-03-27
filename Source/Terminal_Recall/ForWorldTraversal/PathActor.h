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
	UPROPERTY(EditAnywhere, Category="Stuff For Designers | Necessities")
	bool bHelpRefIsGone = false;
	UPROPERTY(EditAnywhere, Category="Stuff For Designers | Necessities", meta = (EditCondition = "bHelpRefIsGone"))
	TSoftObjectPtr<AHarryWorldTraversalSystem> WorldTraversalSystem;
	UPROPERTY(EditAnywhere, Category="Stuff For Designers | Necessities", meta = (EditCondition = "bHelpRefIsGone"))
	TSoftObjectPtr<AHarryWorldTraversalNode> Node;
	UPROPERTY(Transient)
	FHarryNeighbourStruct RelatedStruct;
	
public:
	void LinkStruct(AHarryWorldTraversalSystem* system, AHarryWorldTraversalNode* owningNode, FHarryNeighbourStruct relatedStruct);
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
};
