#include "PlayerBattleController.h"

#include "Blueprint/UserWidget.h"
#include "Terminal_Recall/StuffForUI/BattleChoiceSelectUI.h"
#include "Terminal_Recall/StuffForUI/MainBattleUI.h"

void APlayerBattleController::BeginPlay()
{
	Super::BeginPlay();
	bEnableMouseOverEvents = false;
	MainBattleUIWidget = Cast<UMainBattleUI>(CreateWidget(this, MainBattleUIClass));
	MainBattleUIWidget->AddToViewport();
	BattleChoiceUIWidget = Cast<UBattleChoiceSelectUI>(CreateWidget(this, BattleChoiceUIClass));
}
