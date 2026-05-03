#pragma once

#include "CoreMinimal.h"
#include "BaseInteractable.h"
#include "BaseItemInteractable.generated.h"

UCLASS()
class TERMINAL_RECALL_API ABaseItemInteractable : public ABaseInteractable
{
	GENERATED_BODY()
	
public:
	ABaseItemInteractable();
	
	// Basic information for the interactable object, exposed to Blueprints
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable|Info")
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable|Info")
	FString ItemDescription;

	//Index to get from DataList
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Interactable|Info")
	int32 AttackIndex = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Interactable|Info")
	int32 MoveIndex = -1;

	// Use confirmation UI func when player attempts to interact
	// BlueprintImplementableEvent allows designers to build the UI logic entirely in Blueprints
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interactable|Interaction")
	void ShowConfirmationUI();
	virtual void ShowConfirmationUI_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable|Interaction")
	void OnItemUse();
	virtual void OnItemUse_Implementation();

	// Function to pass info (name and description)
	// BlueprintPure makes it a green node in Blueprints (no execution pins needed)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interactable|Info")
	virtual void GetItemInfo(FString& OutName, FString& OutDescription) const;
	
	virtual void OnActivate_Implementation(ABattleEntity* entity) override; 
};
