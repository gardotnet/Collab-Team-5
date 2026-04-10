#include "DefaultInstance.h"

#if WITH_EDITOR
void UDefaultInstance::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UDefaultInstance, AttackList))
	{
		if (AttackList.Num() > 5)
			AttackList.SetNum(5);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UDefaultInstance, MoveList))
	{
		if (MoveList.Num() > 5)
			MoveList.SetNum(5);
	}
}
#endif

void UDefaultInstance::OnStart()
{
	Super::OnStart();
	AttackList.SetNum(5);
	MoveList.SetNum(5);
}

void UDefaultInstance::EncounterWon()
{
	VisitedCells.Add(PlayerSpawnPos);
}

void UDefaultInstance::AddVisitedCells(FVector newPos)
{
	VisitedCells.Add(newPos);
}
