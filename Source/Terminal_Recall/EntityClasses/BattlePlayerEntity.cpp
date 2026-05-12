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
		GI = gameInstance;
		
		FindAndSaveActions(gameInstance->AttackList, gameInstance->MoveList, false);

		CurrentHealth = gameInstance->Health;
		MaxHealth = gameInstance->MaxHealth;
		DamageMultiplier = gameInstance->DamageMultiplier;
	}

	EquipMechPart();
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

void ABattlePlayerEntity::EquipMechPart() {
	bool bAnyPartEnabled = false;

	UE_LOG(LogTemp, Warning, TEXT("[CURRENT PLAYER PARTS] Arms=%d Legs=%d Booster=%d Wings=%d"),
		IsMechPartUnlocked(true, ATK_ArmsIndex),
		IsMechPartUnlocked(true, ATK_LegsIndex),
		IsMechPartUnlocked(true, ATK_BoosterIndex),
		IsMechPartUnlocked(false, MOVE_WingsIndex)
	);
	
	if (IsMechPartUnlocked(true, ATK_ArmsIndex) || IsMechPartUnlocked(false,MOVE_ArmsIndex)) {
		R_ArmMesh->SetHiddenInGame(false);
		L_ArmMesh->SetHiddenInGame(false);

		bAnyPartEnabled = true;
	}

	if (IsMechPartUnlocked(true, ATK_LegsIndex) || IsMechPartUnlocked(false,MOVE_LegsIndex)) {
		R_LegMesh->SetHiddenInGame(false);
		L_LegMesh->SetHiddenInGame(false);

		bAnyPartEnabled = true;
	}

	if (IsMechPartUnlocked(true, ATK_BoosterIndex) || IsMechPartUnlocked(false,MOVE_BoosterIndex)) {
		BoostersMesh->SetHiddenInGame(false);

		bAnyPartEnabled = true;
	}

	if (IsMechPartUnlocked(false,MOVE_WingsIndex)) {
		WindshieldMesh->SetHiddenInGame(false);

		bAnyPartEnabled = true;
	}

	MouseMesh->SetHiddenInGame(bAnyPartEnabled);
	
	CockpitMesh->SetHiddenInGame(!bAnyPartEnabled);
	CharaMesh->SetHiddenInGame(!bAnyPartEnabled);
}

bool ABattlePlayerEntity::IsMechPartUnlocked(bool bIsAttack, int32 Index) const{
	if (!GI) return false;

	const TArray<int32>& List = bIsAttack
		? GI->AttackList
		: GI->MoveList;

	return List.IsValidIndex(Index) && List[Index] != INDEX_NONE;
}