#pragma once

#include "CoreMinimal.h"
#include "GridRow.generated.h"

USTRUCT(BlueprintType)
struct FGridRow
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Elements;

	void Add(int32 element) { Elements.Add(element); }
	int32 Num() { return Elements.Num(); }
	int32& operator[](int32 element) { return Elements[element]; }
};
