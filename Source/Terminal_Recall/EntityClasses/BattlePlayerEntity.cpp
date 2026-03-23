#include "BattlePlayerEntity.h"

#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/EncounterRelatedCode/BattleManager.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/DefaultInstance.h"

ABattlePlayerEntity::ABattlePlayerEntity()
{
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent->SetCollisionProfileName("Player");
}
void ABattlePlayerEntity::OnBattleStart(ABattleManager* battleManager, int32 enemyID)
{
	BattleManager = battleManager;
	if (UDefaultInstance* gameInstance =  Cast<UDefaultInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		FindAndSaveActions(gameInstance->AttackList, gameInstance->MoveList, false);
		CurrentHealth = gameInstance->Health;
		MaxHealth = gameInstance->MaxHealth;
		DamageMultiplier = gameInstance->DamageMultiplier;
	}
}