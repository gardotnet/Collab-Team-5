#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TraversalFunctionLibrary.generated.h"

class USplineComponent;
class AActor;

UCLASS()
class TERMINAL_RECALL_API UTraversalFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Traversal System | Movement")
	static bool EvaluateSplineMovement(USplineComponent* Spline, float Progress, bool bForward, FVector& OutLocation, FRotator& OutRotation);
	
	UFUNCTION(BlueprintCallable, Category="Traversal System | Movement")
	static void CalculateSplineMoveDuration(USplineComponent* Spline, float Speed, float& OutDuration);
};
