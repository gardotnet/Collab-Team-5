#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Terminal_Recall/StructsAndEnums/ActionTypes.h"
#include "ActionButtonUI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionSelected, UActionButtonUI*, action);

class ABattleEntity;
class UScrollBox;
class UButton;

UCLASS()
class TERMINAL_RECALL_API UActionButtonUI : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UPROPERTY()
	TObjectPtr<ABattleEntity> Entity;
	FString ActionName;
	EActionType ActionType;
	int32 IndexInOwningEntityArray = -1;
	
	FOnActionSelected OnActionSelected;
	
protected:
	UPROPERTY(meta = (BindWidget))
	UButton* ActionButton;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void OnButtonClicked();
	
};
