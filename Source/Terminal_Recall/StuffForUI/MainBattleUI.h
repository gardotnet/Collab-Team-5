// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainBattleUI.generated.h"

class UHorizontalBox;
class UTextBlock;

UCLASS()
class TERMINAL_RECALL_API UMainBattleUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* HealthText;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* EnemyHealthText;
	UPROPERTY(meta=(BindWidget))
	UHorizontalBox* EnemyHPContainer;
	
public:
	UFUNCTION(BlueprintCallable, Category="For Designers | Functions")
	void ChangeHealth(int32 newHealthValue);
	
	UFUNCTION(BlueprintCallable, Category="For Designers | Functions")
	void SetEnemyHp(int32 newHealthValue);
	
	UFUNCTION(BlueprintCallable, Category="For Designers | Functions")
	void HideEnemyHp();
	
protected:
	virtual void NativeConstruct() override;
	
};
