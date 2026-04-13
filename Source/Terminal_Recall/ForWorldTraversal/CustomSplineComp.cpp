#include "CustomSplineComp.h"

UCustomSplineComp::UCustomSplineComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}
#if WITH_EDITOR
void UCustomSplineComp::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	// This WILL run in the editor when you change properties
	UE_LOG(LogTemp, Warning, TEXT("Spline Edited in Editor!"));
}
#endif

