// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Terminal_Recall/StructsAndEnums/ForDataTable/TrapInfoStruct.h"
#include "BaseTrapLatest.generated.h"

UCLASS(Blueprintable, BlueprintType)
class TERMINAL_RECALL_API ABaseTrapLatest : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseTrapLatest();

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
};
