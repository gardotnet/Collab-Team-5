#include "PlayerBattleController.h"

#include "Blueprint/UserWidget.h"
#include "Terminal_Recall/StuffForUI/BattleChoiceSelectUI.h"

void APlayerBattleController::BeginPlay()
{
	Super::BeginPlay();
	bEnableMouseOverEvents = false;
	BattleChoiceUIWidget = Cast<UBattleChoiceSelectUI>(CreateWidget(this, BattleChoiceUIClass));
}
