#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleLevelLoader.generated.h"

class ABattleEnemyEntity;
class ABattlePlayerEntity;
class ABattleManager;
class ABattleGridCell;

UCLASS()
class TERMINAL_RECALL_API ABattleLevelLoader : public AActor
{
	GENERATED_BODY()

public:
	ABattleLevelLoader();

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "For Designers | Necessities")
	TObjectPtr<UDataTable> EnemyDataTable;
	UPROPERTY(EditDefaultsOnly, Category = "For Designers | Necessities")
	TObjectPtr<UDataTable> TrapDataTable;
	UPROPERTY(EditDefaultsOnly, Category = "For Designers | Necessities")
	TObjectPtr<UDataTable> InteractableDataTable;
	
	UPROPERTY(EditAnywhere, Category="For Designers | Necessities")
	TSubclassOf<ABattleGridCell> BattleGridClass;
	UPROPERTY(EditAnywhere, Category="For Designers | Necessities")
	TSubclassOf<ABattleManager> BattleManagerClass;
	UPROPERTY(EditAnywhere, Category="For Designers | Necessities")
	TSubclassOf<AActor> WallClass;
	UPROPERTY(EditAnywhere, Category="For Designers | Necessities")
	TSubclassOf<ABattlePlayerEntity> PlayerEntityClass;
	UPROPERTY(EditAnywhere, Category="For Designers | Necessities")
	TSubclassOf<ABattleEnemyEntity> BaseEnemyEntityClass;
	
	UPROPERTY(EditAnywhere, Category="For Designers | Necessities")
	UMaterialInterface* CheckBoardMat1;
	UPROPERTY(EditAnywhere, Category="For Designers | Necessities")
	UMaterialInterface* CheckBoardMat2;
	UPROPERTY(EditAnywhere, Category="For Designers | Necessities")
	UMaterialInterface* MoveHighlightMat;
	
	virtual void BeginPlay() override;
	
	void LoadLevel(FString levelPath);
	
};
