#include "BattleEnemyEntity.h"

#include "BattlePlayerEntity.h"
#include "Terminal_Recall/EncounterRelatedCode/BattleManager.h"
#include "Terminal_Recall/GridCellClasses/BattleGridCell.h"
#include "Terminal_Recall/StructsAndEnums/ForDataTable/EnemyInfoStruct.h"

ABattleEnemyEntity::ABattleEnemyEntity()
{
	PrimaryActorTick.bCanEverTick = true;
}
void ABattleEnemyEntity::OnTurnGo()
{
	OnMoveDoneEvent.Broadcast();
	
	bool inRange = false;
	for (auto pos : AttackRange)
	{
		if (TObjectPtr<ABattleGridCell>* cell = BattleManager->GridMap.Find(pos))
		{
			if (Cast<ABattlePlayerEntity>((*cell)->GetEntity()))
			{
				inRange = true;
				break;
			}
		}
	}
	// when within range
	if (inRange)
	{
		int32 RandomInt = FMath::RandRange(1, 100);
		if (RandomInt < 30)
		{
			UE_LOG(LogTemp, Warning, TEXT("Nah i move"))
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Should have avoided my range"))
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Moving"))
	}
	// Decrement all cooldowns by 1
	BattleManager->NextEntityTurn();
}

void ABattleEnemyEntity::OnBattleStart(ABattleManager* battleManager, int32 enemyID)
{
	BattleManager = battleManager;
	FString enemyIDString = FString::FromInt(enemyID);
	if (!EnemyDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Where the enemy data table?"))
		return;
	}
	if (FEnemyInfoStruct* enemyInfo = EnemyDataTable->FindRow<FEnemyInfoStruct>(FName(*enemyIDString), TEXT("Entity Search")))
	{
		FindAndSaveActions(enemyInfo->AttackIdList, enemyInfo->MovementIdList);
		CurrentHealth = enemyInfo->Health;
		MaxHealth = enemyInfo->Health;
		DamageMultiplier = 1;
	}
	for (auto attacks : AttackInfoList)
	{
		for (auto cellInfo : attacks.Cells)
		{
			AttackRange.AddUnique(cellInfo.GridPos);
		}
	}
}


