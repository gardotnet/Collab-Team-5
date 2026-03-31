#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Terminal_Recall/StructsAndEnums/ActionTypes.h"
#include "ActionButtonUI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionSelected, UActionButtonUI*, action);

class ABattleEntity;
class UScrollBox;
class UButton;
class UTextBlock;

UCLASS()
class TERMINAL_RECALL_API UActionButtonUI : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta = (BindWidget))
	UButton* ActionButton;
public:
	UPROPERTY()
	TObjectPtr<ABattleEntity> Entity;
	FString ActionName;
	EActionType ActionType;
	int32 IndexInOwningEntityArray = -1;
	FOnActionSelected OnActionSelected;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameLabel;
	int32 Cooldown = 0;
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	UFUNCTION()
	void OnButtonClicked();
public:
	UFUNCTION()
	void RemoveForChoices();
	void SetCooldown(int32 cooldown);
};
