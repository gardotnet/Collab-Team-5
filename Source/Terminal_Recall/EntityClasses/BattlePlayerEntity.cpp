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

	MouseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MouseMesh"));
	MouseMesh->SetupAttachment(MeshComponent);

	CharaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CharaMesh"));
	CharaMesh->SetupAttachment(MeshComponent);

	R_ArmMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightArmMesh"));
	R_ArmMesh->SetupAttachment(MeshComponent);

	L_ArmMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftArmMesh"));
	L_ArmMesh->SetupAttachment(MeshComponent);

	R_LegMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightLegMesh"));
	R_LegMesh->SetupAttachment(MeshComponent);

	L_LegMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftLegMesh"));
	L_LegMesh->SetupAttachment(MeshComponent);

	BoostersMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoostersMesh"));
	BoostersMesh->SetupAttachment(MeshComponent);

	CockpitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CockpitMesh"));
	CockpitMesh->SetupAttachment(MeshComponent);

	WindshieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WindshieldMesh"));
	WindshieldMesh->SetupAttachment(MeshComponent);

	//Hide mech parts on start:
	CharaMesh->SetHiddenInGame(true);
	R_ArmMesh->SetHiddenInGame(true);
	L_ArmMesh->SetHiddenInGame(true);
	R_LegMesh->SetHiddenInGame(true);
	L_LegMesh->SetHiddenInGame(true);
	BoostersMesh->SetHiddenInGame(true);
	CockpitMesh->SetHiddenInGame(true);
	WindshieldMesh->SetHiddenInGame(true);

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