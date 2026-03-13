#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathReseter.generated.h"

UCLASS()
class TERMINAL_RECALL_API APathReseter : public AActor
{
	GENERATED_BODY()

public:
	APathReseter();
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Stuff For Designers | Grid Cell Tool")
	void ResetPositionForAllPath();
};
