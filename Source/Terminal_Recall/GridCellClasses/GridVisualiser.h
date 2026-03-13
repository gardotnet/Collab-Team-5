#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Terminal_Recall/StructsAndEnums/For2dArray/LevelCellData.h"
#include "GridVisualiser.generated.h"

class UInstancedStaticMeshComponent;

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

	UFUNCTION(BlueprintCallable, Category = "For Designers | General Info")
	void RegenerateGrid(FIntPoint gridSize);
	
	
};
