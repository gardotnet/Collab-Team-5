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

	//initialise lists with -1 (empty/unused slot)
	AttackList.Init(-1, 5);
	MoveList.Init(-1,5);

	//set king move from DT_ActionList at index 0
	AttackList[0] = 1;
	MoveList[0] = 1;

	/*
	AttackList.SetNum(5);
	MoveList.SetNum(5);
	*/

}

void UDefaultInstance::EncounterWon()
{
	VisitedCells.Add(PlayerSpawnPos);
}