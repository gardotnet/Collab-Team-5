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
	// data tables
	UPROPERTY(EditDefaultsOnly, Category = "Stuff For Designers | Necessities")
	TObjectPtr<UDataTable> PropDataTable;
	UPROPERTY(EditDefaultsOnly, Category = "Stuff For Designers | Necessities")
	TObjectPtr<UDataTable> TrapDataTable;
	UPROPERTY(EditDefaultsOnly, Category = "Stuff For Designers | Necessities")
	TObjectPtr<UDataTable> InteractableDataTable;
	// base classes
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Necessities")
	TSubclassOf<ABattleGridCell> BattleGridClass;
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Necessities")
	TSubclassOf<ABattleManager> BattleManagerClass;
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Necessities")
	TSubclassOf<ABattlePlayerEntity> PlayerEntityClass;
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Necessities")
	TSubclassOf<ABattleEnemyEntity> BaseEnemyEntityClass;
	// material for grid colouring
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Necessities")
	UMaterialInterface* CheckBoardMat1;
	UPROPERTY(EditDefaultsOnly, Category="Stuff For Designers | Necessities")
	UMaterialInterface* CheckBoardMat2;
	
	FString OverwriteLevelPath = "";
	TMap<FIntPoint, TObjectPtr<ABattleGridCell>> TempGridMap;
	TArray<AActor*> TempActors;
protected:
	virtual void BeginPlay() override;
	void LoadLevel(FString levelPath);
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Stuff For Designers | Functions")
	void EditorLoadLevel();
};
