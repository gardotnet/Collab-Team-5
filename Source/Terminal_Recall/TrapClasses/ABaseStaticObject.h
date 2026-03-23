#pragma once

#include "CoreMinimal.h"
#include "BaseInteractableObj.h"
#include "ABaseStaticObject.generated.h"

UCLASS()
class TERMINAL_RECALL_API ABaseStaticObject : public ABaseInteractableObj
{
	GENERATED_BODY()
	
public:
	ABaseStaticObject();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Static Object")
	bool bProvidesCover;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Static Object")
	bool bBlocksMovement;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Static Object")
	void InitializeStaticObject();
};
