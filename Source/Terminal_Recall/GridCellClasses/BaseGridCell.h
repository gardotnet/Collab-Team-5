#pragma once

#include "CoreMinimal.h"
#include "Terminal_Recall/EntityClasses/BaseEntity.h"
#include "BaseGridCell.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class TERMINAL_RECALL_API ABaseGridCell : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BoxComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stuff For Designers | Grid Cell Info | General Stuff")
	bool Clickable;
	UPROPERTY()
	TObjectPtr<ABaseEntity> UnitRef;
protected:
	ABaseGridCell();
	virtual void BeginPlay() override;
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stuff For Designers | Functions")
	void OnUnitArrive();
	virtual void OnUnitArrive_Implementation();
private:
	UFUNCTION()
	virtual void OnCellClicked(AActor* TouchedActor, FKey ButtonPressed);
};
