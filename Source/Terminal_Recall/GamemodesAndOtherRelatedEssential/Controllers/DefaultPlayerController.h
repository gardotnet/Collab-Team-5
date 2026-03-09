#pragma once

#include "CoreMinimal.h"

#include "DefaultPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TERMINAL_RECALL_API ADefaultPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
};
