// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "CustomSplineComp.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TERMINAL_RECALL_API UCustomSplineComp : public USplineComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCustomSplineComp();
	
	#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
	#endif
};
