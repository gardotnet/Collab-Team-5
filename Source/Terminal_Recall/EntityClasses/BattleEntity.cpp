#include "BattleEntity.h"

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

void ABattleEntity::OnMovementSelected()
{
	// if player clicked movement button
}

void ABattleEntity::OnAttackSelected(int index)
{
	// if player picked one of the attack buttons
}

void ABattleEntity::OnAttack(int id, FVector3f pos)
{
	// finds the attack in the Global Attack List and play the attack animation
}

void ABattleEntity::ShowChoices()
{
	// add all buttons to the UI
}

void ABattleEntity::HideChoices()
{
	// removes all button from the UI
}

void ABattleEntity::OnMove(FVector3f pos)
{
	// check if entity can move on that grid
}

void ABattleEntity::OnEntityClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	UE_LOG(LogTemp, Warning, TEXT("Hey! I'm snitching to the battle manager now!"));
}




