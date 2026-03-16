#pragma once

#include "CoreMinimal.h"
#include "OurBasePlayerController.h"
#include "PlayerBattleController.generated.h"

UCLASS()
class TERMINAL_RECALL_API APlayerBattleController : public AOurBasePlayerController
{
	GENERATED_BODY()
public:
	void DisplayMovesUI();
};
