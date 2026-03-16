#include "OurBasePlayerController.h"

void AOurBasePlayerController::BeginPlay()
{

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	TimerManagerRef = &GetWorld()->GetTimerManager();
	MoveTimerHandle = FTimerHandle();
}

void AOurBasePlayerController::MovePlayer()
{
}
