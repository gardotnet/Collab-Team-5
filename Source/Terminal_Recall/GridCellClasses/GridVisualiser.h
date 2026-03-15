#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridVisualiser.generated.h"

class UTextRenderComponent;
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
	
	TMap<int32, TObjectPtr<UTextRenderComponent>> TextComponentList;

	void RegenerateGrid(FIntPoint gridSize);
	void AddText(int32 ismIndex, FText text, FIntPoint groupSize);
	void SetText(int32 ismIndex, FText text, FIntPoint groupSize);
	void DeleteText(int32 ismIndex);
	void ClearText();
};
