#include "TraversalFunctionLibrary.h"
#include "Components/SplineComponent.h"

bool UTraversalFunctionLibrary::EvaluateSplineMovement(USplineComponent* Spline, float Progress, bool bForward, FVector& OutLocation, FRotator& OutRotation)
{
	if (!IsValid(Spline))
	{
		return false;
	}

	float SplineLength = Spline->GetSplineLength();
	float ClampedProgress = FMath::Clamp(Progress, 0.0f, 1.0f);

	float CurrentDistance = bForward ? 
		FMath::Lerp(0.0f, SplineLength, ClampedProgress) : 
		FMath::Lerp(SplineLength, 0.0f, ClampedProgress);

	OutLocation = Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
	OutRotation = Spline->GetRotationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
			
	// If moving backwards, flip the rotation so the entity faces the direction it is moving
	if (!bForward)
	{
		OutRotation.Yaw += 180.0f;
	}

	return true;
}

void UTraversalFunctionLibrary::CalculateSplineMoveDuration(USplineComponent* Spline, float Speed, float& OutDuration)
{
	if (!IsValid(Spline) || Speed <= 0.0f)
	{
		OutDuration = 0.0f;
		return;
	}

	float SplineLength = Spline->GetSplineLength();
	OutDuration = SplineLength / Speed;
}
