#include "BattleEntity.h"

#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/EncounterRelatedCode/BattleManager.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/Controllers/PlayerBattleController.h"
#include "Terminal_Recall/StuffForUI/BattleChoiceSelectUI.h"

ABattleEntity::ABattleEntity()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABattleEntity::OnBattleStart(ABattleManager* battleManager, float health, float damageMultiplier)
{
	BattleManager = battleManager;
	Health = health;
	DamageMultiplier = damageMultiplier;
}
void ABattleEntity::OnTurnGo()
{
	// Decrement all cooldowns by 1
	ShowChoices();
}

void ABattleEntity::ShowChoices()
{
	// add all buttons to the UI
	PlayerBattleController->BattleChoiceUIWidget->AddActionButton(MovementActionsUI, AttackActionsUI, ItemActionsUI);
}

void ABattleEntity::SetPos(FIntPoint newPos)
{
	OnChangeCell.Broadcast();
	GridPos = newPos;
}

void ABattleEntity::TakeDamage(float damage)
{
	Health = Health - damage;
	if (Health <= 0.0f)
	{
		OnChangeCell.Broadcast();
		Destroy();
	}
}

void ABattleEntity::BeginPlay()
{
	Super::BeginPlay();
	PlayerBattleController = Cast<APlayerBattleController>(GetWorld()->GetFirstPlayerController());
	BattleManager = Cast<ABattleManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ABattleManager::StaticClass()));
}

void ABattleEntity::OnMove()
{
	MoveTimer += 0.01f;
	FVector playerPos = GetActorLocation();
	float distance = FVector::Dist(playerPos, TargetPosition);
	
	if (MoveTimer > DesiredMoveDuration || distance < 1)
	{
		OnMoveDoneEvent.Broadcast();
		TimerManagerRef->ClearTimer(MoveTimerHandle);
		SetActorLocation(TargetPosition);
	}
	else
	{
		SetActorLocation(FMath::Lerp(playerPos, TargetPosition, MoveTimer/DesiredMoveDuration));
	}
}

void ABattleEntity::OnEntityClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	BattleManager->OnEntitySelected(this);
}