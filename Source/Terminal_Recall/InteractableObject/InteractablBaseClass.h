// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractablBaseClass.generated.h"

// Making this class Blueprintable so Designers can derive Blueprint classes from it.
// Abstract prevents spawning the base C++ class directly if it's only meant to be a parent.
UCLASS(Abstract, Blueprintable)
class TERMINAL_RECALL_API AInteractablBaseClass : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteractablBaseClass();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

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
