#pragma once

#include "CoreMinimal.h"
#include "BaseGridCell.h"
#include "WorldGridCell.generated.h"

UCLASS()
class TERMINAL_RECALL_API AWorldGridCell : public ABaseGridCell
{
	GENERATED_BODY()

public:
	AWorldGridCell();

	UPROPERTY(EditInstanceOnly, Category="Grid Cell Info (For Designers)", meta = (ToolTip = "Select the interactable related to this cell here"))
	TObjectPtr<AActor> InteractableRef; // [NOTE] will change to only Interactable Class instead of AActor
};
