#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameStateHandle.generated.h"

class AHarryWorldTraversalNode;
class ATraversalNode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPathsCreated);

UCLASS()
class TERMINAL_RECALL_API AGameStateHandle : public AGameStateBase
{
	GENERATED_BODY()

public:
	// A registry or manager for nodes and their neighbors could be here
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Traversal System")
	TArray<AHarryWorldTraversalNode*> AllNodes;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Traversal System")
	FOnPathsCreated OnPathsCreated;
};
