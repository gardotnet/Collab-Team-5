#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridVisualiser.generated.h"

class UInstancedStaticMeshComponent;

UENUM(BlueprintType)
enum class ELevelCellType : uint8
{
	None UMETA(DisplayName = "None"),
	EnemySpawn UMETA(DisplayName = "EnemySpawn"),
	Trap UMETA(DisplayName = "Trap"),
	ItemSpawn UMETA(DisplayName = "ItemSpawn")
};

USTRUCT(BlueprintType)
struct FLevelCellData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELevelCellType CellType = ELevelCellType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UObject> RequiredObject;
};
UCLASS()
class TERMINAL_RECALL_API AGridVisualiser : public AActor
{
	GENERATED_BODY()

public:
	AGridVisualiser();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInstancedStaticMeshComponent> GridVisualComp;

	UPROPERTY(EditDefaultsOnly, Category="For Designers | General Info")
	float GridWidth = 1;
	UPROPERTY(EditDefaultsOnly, Category="For Designers | General Info")
	float GridHeight = 1;
	UPROPERTY(EditDefaultsOnly, Category="For Designers | General Info")
	float TileSize = 100;

	TMap<FIntPoint, FLevelCellData> GridData;

	UFUNCTION(BlueprintCallable, Category = "For Designers | General Info")
	void RegenerateGrid(FIntPoint gridSize);
	
	
};
