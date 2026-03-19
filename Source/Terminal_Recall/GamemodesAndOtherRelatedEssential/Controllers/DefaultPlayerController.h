#pragma once

#include "CoreMinimal.h"
#include "OurBasePlayerController.h"
#include "DefaultPlayerController.generated.h"

class UInputMappingContext;
class AAIController;
class ABaseEntity;
class UNodeInfoForCellSearch;
class AWorldGridCell;
/**
 * 
 */
UCLASS()
class TERMINAL_RECALL_API ADefaultPlayerController : public AOurBasePlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="Stuff For Designers | General Stuff")
	TSubclassOf<ABaseEntity> PlayerChar;
	UPROPERTY(EditAnywhere, Category="Stuff For Designers | General Stuff")
	float PlayerSpeed = 100;
	UPROPERTY(EditAnywhere, Category="Input Stuff")
	TObjectPtr<UInputMappingContext> MappingContext;
protected:
	UPROPERTY()
	TObjectPtr<ABaseEntity> PlayerEntity;
	UPROPERTY()
	TObjectPtr<AWorldGridCell> CurrentCell;
	UPROPERTY()
	TObjectPtr<AWorldGridCell> DestinationCell;
	int32 PathIndex = 0;
	TArray<TObjectPtr<AWorldGridCell>> ThePath;
	UPROPERTY()
	TArray<TObjectPtr<UNodeInfoForCellSearch>> OpenList;
	UPROPERTY()
	TArray<TObjectPtr<UNodeInfoForCellSearch>> CloseList;

	float MoveTimer = 0.0f;
	float DesiredMoveDuration = 0.0f;
public:
	UFUNCTION()
	virtual void MovePlayer() override;
	void NavigateWorld(TObjectPtr<AWorldGridCell> cell);
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
protected:
	bool FindCell(TObjectPtr<UNodeInfoForCellSearch> currentInfo);
	// Utilities
	float Heuristic_Manhattan(FVector start, FVector end);
};

