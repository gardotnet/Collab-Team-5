#pragma once

#include "CoreMinimal.h"
#include "DefaultPlayerController.generated.h"



class AAIController;
class ABaseEntity;
class UNodeInfoForCellSearch;
class AWorldGridCell;
/**
 * 
 */
UCLASS()
class TERMINAL_RECALL_API ADefaultPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="Variable (For Designers)")
	TObjectPtr<AWorldGridCell> SpawnPointCell;
	UPROPERTY(EditAnywhere, Category="Variable (For Designers)")
	TSubclassOf<ABaseEntity> PlayerChar;
	UPROPERTY(EditAnywhere, Category="Variable (For Designers)")
	float PlayerSpeed = 100;
	
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
	
	FTimerManager* TimerManagerRef;
	FTimerHandle MoveTimerHandle;

	float MoveTimer = 0.0f;
	float DesiredMoveDuration = 0.0f;
	

public:
	UFUNCTION()
	void MovePlayer();
	void NavigateWorld(TObjectPtr<AWorldGridCell> cell);
	
	virtual void BeginPlay() override;
	
protected:
	bool FindCell(TObjectPtr<UNodeInfoForCellSearch> currentInfo);
	
	// Utilities
	float Heuristic_Manhattan(FVector start, FVector end);
	
};

