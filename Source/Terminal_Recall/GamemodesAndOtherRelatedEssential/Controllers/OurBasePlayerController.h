// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OurBasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TERMINAL_RECALL_API AOurBasePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void MovePlayer();
protected:
	FTimerManager* TimerManagerRef;
	FTimerHandle MoveTimerHandle;
};
