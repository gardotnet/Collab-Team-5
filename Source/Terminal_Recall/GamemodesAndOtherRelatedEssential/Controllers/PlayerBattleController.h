#pragma once

#include "CoreMinimal.h"
#include "OurBasePlayerController.h"
#include "PlayerBattleController.generated.h"

class UActionButtonUI;
class UBattleChoiceSelectUI;

UCLASS()
class TERMINAL_RECALL_API APlayerBattleController : public AOurBasePlayerController
{
	GENERATED_BODY()


	
public:
	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	TSubclassOf<UUserWidget> BattleChoiceUIClass;

	UPROPERTY()
	TObjectPtr<UBattleChoiceSelectUI> BattleChoiceUIWidget;
	
protected:
	virtual void BeginPlay() override;
};
