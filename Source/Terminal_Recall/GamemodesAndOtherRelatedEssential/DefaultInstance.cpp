#include "DefaultInstance.h"

#if WITH_EDITOR
void UDefaultInstance::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UDefaultInstance, AttackList))
	{
		if (AttackList.Num() > 7)
			AttackList.SetNum(7);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UDefaultInstance, MoveList))
	{
		if (MoveList.Num() >7)
			MoveList.SetNum(7);
	}
}
#endif

void UDefaultInstance::OnStart()
{
	Super::OnStart();

	//initialise lists with empty/unused slots
	AttackList.Init(INDEX_NONE, 7);
	MoveList.Init(INDEX_NONE,7);

	//set king move from DT_ActionList at index 0
	AttackList[0] = 1;
	MoveList[0] = 1;

}

void UDefaultInstance::EncounterWon()
{
	VisitedCells.Add(PlayerSpawnPos);
}

bool UDefaultInstance::HasFixedMech() const
{
	for (int32 Value : AttackList)
	{
		if (Value == INDEX_NONE)
			return false;
	}

	for (int32 Value : MoveList)
	{
		if (Value == INDEX_NONE)
			return false;
	}

	return true;
}