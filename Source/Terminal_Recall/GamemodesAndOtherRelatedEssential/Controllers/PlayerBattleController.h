#pragma once

#include "CoreMinimal.h"
#include "OurBasePlayerController.h"
#include "PlayerBattleController.generated.h"

class UMainBattleUI;
class UActionButtonUI;
class UBattleChoiceSelectUI;

UCLASS()
class TERMINAL_RECALL_API APlayerBattleController : public AOurBasePlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	TSubclassOf<UUserWidget> BattleChoiceUIClass;
	UPROPERTY(BlueprintReadWrite, Category="Stuff For Designers | Variables")
	TObjectPtr<UUserWidget> BattleChoiceUIWidget;
	
	/*UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	TSubclassOf<UUserWidget> MainBattleUIClass;
	UPROPERTY(BlueprintReadWrite, Category="Stuff For Designers | Variables")
	TObjectPtr<UMainBattleUI> MainBattleUIWidget;*/
	
public:
	
	UFUNCTION(BlueprintCallable, Category="Stuff For Designers | Functions")
	void HideUI();
	
protected:
	virtual void BeginPlay() override;
};
