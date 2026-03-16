#include "BattleEntity.h"

#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/EncounterRelatedCode/BattleManager.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/Controllers/PlayerBattleController.h"

ABattleEntity::ABattleEntity()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABattleEntity::OnBattleStart()
{
	// Create UI buttons and linking it to 'OnAttackSelected' or 'OnMovementSelected'
	// Also store it to show/hide the choices
}

void ABattleEntity::OnTurnGo()
{
	// Decrement all cooldowns by 1
	// (maybe show choices)
}

void ABattleEntity::OnActionSelected()
{
	// [NOTE] if player clicked an option
	// [NOTE] get variables from button and pass it to manager
	if (BattleManager)
	{
		//BattleManager->ActionChoiceSelected()
	}
}

void ABattleEntity::DoAttackAction(TObjectPtr<ABaseEntity> enemy, FVector pos)
{
	if (enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("I refuse to hit this entity"));
		// checks if enemy is dead
		/*if ()
		{
			// moves to opponent position when defeated like chess
			OnMove(pos);
			return;
		}*/
	}
	if (OnTurnDoneEvent.IsBound())
		OnTurnDoneEvent.Broadcast();
}

void ABattleEntity::ShowChoices()
{
	// add all buttons to the UI
}

void ABattleEntity::HideChoices()
{
	// removes all button from the UI
}

void ABattleEntity::BeginPlay()
{
	Super::BeginPlay();
	PlayerBattleController = Cast<APlayerBattleController>(GetWorld()->GetFirstPlayerController());
	BattleManager = Cast<ABattleManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ABattleManager::StaticClass()));
}

void ABattleEntity::OnMove()
{
	// check if entity can move on that grid
}

void ABattleEntity::OnEntityClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	BattleManager->OnEntitySelected(this);
}