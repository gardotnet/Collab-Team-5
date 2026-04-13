#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "WorldTraversalEntity.generated.h"

class AWorldGridCell;
class AOurBasePlayerController;

UCLASS()
class TERMINAL_RECALL_API AWorldTraversalEntity : public ABaseEntity
{
	GENERATED_BODY()

public:
	AWorldTraversalEntity();
	UPROPERTY()
	TObjectPtr<ADefaultPlayerController> PlayerController;
	virtual void StartMove(TArray<TObjectPtr<AWorldGridCell>> path) override;
	
protected:
	
	TArray<TObjectPtr<AWorldGridCell>> TheCellPath;
	int32 PathIndex;
	// path node list
	
	virtual void BeginPlay() override;
	virtual void OnMove() override;
};