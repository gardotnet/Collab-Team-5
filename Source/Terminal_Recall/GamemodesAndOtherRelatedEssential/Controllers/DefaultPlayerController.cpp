#include "DefaultPlayerController.h"

void ADefaultPlayerController::BeginPlay()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}
