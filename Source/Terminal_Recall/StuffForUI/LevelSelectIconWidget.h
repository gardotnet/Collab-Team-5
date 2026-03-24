#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelSelectIconWidget.generated.h"

class UImage;

UCLASS()
class TERMINAL_RECALL_API ULevelSelectIconWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(meta=(BindWidget))
	UImage* IconImage;
	
	UFUNCTION(BlueprintCallable)
	void HideUI();
};
