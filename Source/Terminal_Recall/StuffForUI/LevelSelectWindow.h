#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelSelectWindow.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClearLevelSelectInfoDelegate);

class ULevelSelectIconWidget;
class UHorizontalBox;
class UButton;

UCLASS()
class TERMINAL_RECALL_API ULevelSelectWindow : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Necessities")
	TSubclassOf<ULevelSelectIconWidget> IconWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Necessities")
	TObjectPtr<UTexture2D> DifficultIconTexture;
	UPROPERTY(EditDefaultsOnly, Category = "Stuff For Designers | Necessities")
	TObjectPtr<UDataTable> EnemyDataTable;
	
	FClearLevelSelectInfoDelegate ClearUIEvent;
	
	UPROPERTY()
	TArray<ULevelSelectIconWidget*> DifficultWidgetHolder;
	UPROPERTY()
	TArray<ULevelSelectIconWidget*> EnemyIconWidgetHolder;
	
public:
	
	UPROPERTY(meta=(BindWidget))
	UButton* CloseButton;
	UPROPERTY(meta=(BindWidget))
	UButton* EnterLevelButton;
	
	UPROPERTY(meta=(BindWidget))
	UHorizontalBox* DifficultyContainer;
	UPROPERTY(meta=(BindWidget))
	UHorizontalBox* EnemyHintContainer;

protected:
	
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnCloseButtonPressed();
	UFUNCTION()
	void OnEnterLevelButtonPressed();
	
public:
	UFUNCTION(BlueprintCallable)
	void PassInfoToLevelSelectWindow(int32 difficultyLevel, const TArray<int32>& enemiesID);
};
