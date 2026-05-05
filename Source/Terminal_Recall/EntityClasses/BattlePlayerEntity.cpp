#include "BattlePlayerEntity.h"

#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/EncounterRelatedCode/BattleManager.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/DefaultInstance.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/Controllers/PlayerBattleController.h"
#include "Terminal_Recall/StuffForUI/MainBattleUI.h"

ABattlePlayerEntity::ABattlePlayerEntity()
{
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent->SetCollisionProfileName("Player");
}
void ABattlePlayerEntity::OnBattleStart(ABattleManager* battleManager)
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

void ABattlePlayerEntity::StartMove(FVector position)
{
	Super::StartMove(position);
	
}

void ABattlePlayerEntity::CustomTakeDamage(float damage)
{
	CurrentHealth = CurrentHealth - damage;
	OnDamagedEvent.Broadcast(CurrentHealth);
	if (CurrentHealth <= 0.0f)
	{
		OnChangeCell.Broadcast();
		Destroy();
	}
}