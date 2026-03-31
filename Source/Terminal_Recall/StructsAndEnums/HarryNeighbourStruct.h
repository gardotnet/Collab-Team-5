#pragma once

#include "CoreMinimal.h"
#include "Terminal_Recall/ForWorldTraversal/CustomSplineComp.h"
#include "UObject/Object.h"
#include "HarryNeighbourStruct.generated.h"

class APathActor;
class AHarryWorldTraversalNode;

USTRUCT(BlueprintType)
struct FHarryNeighbourStruct
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AHarryWorldTraversalNode> Node;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(tooltip = "Scaled by 1 Block (100 Units)"))
	int32 Length = 1;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool HasPathConnected = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<APathActor> PathObject;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UCustomSplineComp> PathSplineComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<AActor>> SpawnedPathActors;
};
