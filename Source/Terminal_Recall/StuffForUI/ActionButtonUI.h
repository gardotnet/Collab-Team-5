#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Terminal_Recall/StructsAndEnums/ActionTypes.h"
#include "ActionButtonUI.generated.h"


class UScrollBox;
class UButton;

UCLASS()
class TERMINAL_RECALL_API UActionButtonUI : public UUserWidget
{
	GENERATED_BODY()

public:
	FString ActionName;
	EActionType ActionType;
	int32 IdForAction;
	
	EAttackActionTypes AttackType;
	
protected:
	UPROPERTY(meta = (BindWidget))
	UButton* ActionButton;

protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnButtonClicked();
	
};
