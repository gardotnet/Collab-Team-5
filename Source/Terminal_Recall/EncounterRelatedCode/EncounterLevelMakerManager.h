#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EncounterLevelMakerManager.generated.h"

#if WITH_EDITOR

class AGridVisualiser;

UCLASS()
class TERMINAL_RECALL_API AEncounterLevelMakerManager : public AActor
{
	GENERATED_BODY()

public:
	AEncounterLevelMakerManager();

	#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category="For Designers | Necessities")
	TSubclassOf<AGridVisualiser> GridDisplayerClass;

	UPROPERTY()
	TObjectPtr<AGridVisualiser> GridDisplayerObject;

	FIntPoint GridSize = FIntPoint(10, 10);
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category= "For Designers | Tools")
	void ResetGrid();
	
	UFUNCTION(BlueprintCallable, Category= "For Designers | Functions")
	void CreateGrid(FIntPoint gridSize);

	virtual void Destroyed() override;
	
	
	#endif
};
#endif