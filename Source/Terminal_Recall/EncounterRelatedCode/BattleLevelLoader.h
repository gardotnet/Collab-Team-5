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
	
	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	TSubclassOf<ABattleGridCell> BattleGridClass;
	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	TSubclassOf<ABattleManager> BattleManagerClass;
	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	TSubclassOf<AActor> WallClass;
	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	TSubclassOf<ABattlePlayerEntity> PlayerEntityClass;
	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	TSubclassOf<ABattleEnemyEntity> BaseEnemyEntityClass;
	
	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	UMaterialInterface* CheckBoardMat1;
	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	UMaterialInterface* CheckBoardMat2;
	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	UMaterialInterface* MoveHighlightMat;

	UPROPERTY(EditAnywhere, Category="For Designers | For Editor Tool")
	FString MapFilePath;
	
	virtual void BeginPlay() override;
	
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "For Designers | Functions")
	void EditorLoadLevel();
	void LoadLevel(FString levelPath);
	
};
