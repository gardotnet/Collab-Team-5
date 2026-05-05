#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleChoiceSelectUI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRemoveChoicesDelegate);

class APlayerBattleController;
class UActionButtonUI;
class UScrollBox;
class UButton;

UCLASS()
class TERMINAL_RECALL_API UBattleChoiceSelectUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* CloseButton;
	UPROPERTY(meta = (BindWidget))
	UButton* MoveTabButton;
	UPROPERTY(meta = (BindWidget))
	UButton* AttackTabButton;
	UPROPERTY(meta = (BindWidget))
	UButton* ItemTabButton;

	FRemoveChoicesDelegate OnRemoveChoices;
	
	UPROPERTY()
	TObjectPtr<APlayerBattleController> PlayerBattleController;
	
public:
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Customisation")
	FLinearColor SelectColour = FLinearColor(0.426f, 0.426f, 0.426f, 1.0f);
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Customisation")
	FLinearColor UnselectColour = FLinearColor(0.135f, 0.135f, 0.135f, 1.0f);
	
	UPROPERTY(meta = (BindWidget))
	UScrollBox* MovesScrollBox;
	UPROPERTY(meta = (BindWidget))
	UScrollBox* AttacksScrollBox;
	UPROPERTY(meta = (BindWidget))
	UScrollBox* ItemsScrollBox;
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	UFUNCTION()
	void OnMoveTabButtonClicked();
	UFUNCTION()
	void OnAttackTabButtonClicked();
	UFUNCTION()
	void OnItemTabButtonClicked();
	void UnselectTab();
public:
	UFUNCTION()
	void OnCloseButtonClicked();
	void AddActionButton(TArray<UActionButtonUI*>& movementList, TArray<UActionButtonUI*>& attackList, TArray<UActionButtonUI*>& itemList);
};
