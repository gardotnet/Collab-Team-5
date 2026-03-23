// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Terminal_Recall/StructsAndEnums/ForDataTable/TrapInfoStruct.h"
#include "Terminal_Recall/Interfaces/IHealthInteractable.h"
#include "BaseInteractableObj.generated.h"

UCLASS(Abstract, Blueprintable)
class TERMINAL_RECALL_API ABaseInteractableObj : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseInteractableObj();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "For Designers | Necessities")
	TObjectPtr<UDataTable> TrapDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "For Designers | Necessities")
	FName TrapRowName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "For Designers | Necessities")
	FTrapInfoStruct TrapData;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Trap Events")
	virtual void FetchTrapData();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Trap Events")
	void OnUnitArrive();

	// Basic information for the interactable object, exposed to Blueprints
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable|Info")
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable|Info")
	FString ItemDescription;

	// Use confirmation UI func when player attempts to interact
	// BlueprintImplementableEvent allows designers to build the UI logic entirely in Blueprints
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interactable|Interaction")
	void ShowConfirmationUI();

	// Callable pick up function
	// BlueprintNativeEvent allows C++ to provide a default behavior (like Destroy) 
	// while giving designers the option to override it in Blueprints.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable|Interaction")
	void PickUp();
	virtual void PickUp_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Interactable|Interaction")
	void DamageEntity(AActor* InteractedActor, float amount);

	// Function to pass info (name and description)
	// BlueprintPure makes it a green node in Blueprints (no execution pins needed)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interactable|Info")
	virtual void GetInteractableInfo(FString& OutName, FString& OutDescription) const;

	// Function anyone can edit to make interactable do different things
	// BlueprintNativeEvent allows overriding in Blueprints to implement custom interactions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable|Interaction")
	void OnInteract();
	virtual void OnInteract_Implementation();
};
