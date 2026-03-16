#include "PlayerBattleController.h"

#include "Blueprint/UserWidget.h"
#include "Terminal_Recall/StuffForUI/BattleChoiceSelectUI.h"

void APlayerBattleController::BeginPlay()
{
	Super::BeginPlay();
	BattleChoiceUIWidget = Cast<UBattleChoiceSelectUI>(CreateWidget(this, BattleChoiceUIClass));
}
