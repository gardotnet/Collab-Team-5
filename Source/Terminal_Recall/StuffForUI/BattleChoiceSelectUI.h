#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Terminal_Recall/StructsAndEnums/ActionTypes.h"
#include "BattleChoiceSelectUI.generated.h"

class UActionButtonUI;
class UScrollBox;
class UButton;
/**
 * 
 */
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
	
public:
	UPROPERTY(EditDefaultsOnly, Category="For Designers | Customisation")
	FLinearColor SelectColour = FLinearColor(0.426f, 0.426f, 0.426f, 1.0f);
	UPROPERTY(EditDefaultsOnly, Category="For Designers | Customisation")
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
	void RemoveChoices();

public:
	UFUNCTION()
	void OnCloseButtonClicked();
	
	void AddActionButton(TArray<UActionButtonUI*>& movementList, TArray<UActionButtonUI*>& attackList, TArray<UActionButtonUI*>& itemList);
};
