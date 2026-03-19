#include "BattleEntity.h"

#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "JsonObjectConverter.h"
#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/EncounterRelatedCode/BattleManager.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/Controllers/PlayerBattleController.h"
#include "Terminal_Recall/StuffForUI/BattleChoiceSelectUI.h"

ABattleEntity::ABattleEntity()
{
	PrimaryActorTick.bCanEverTick = true;
}
void ABattleEntity::OnBattleStart(ABattleManager* battleManager, float health, float damageMultiplier)
{
	BattleManager = battleManager;
	CurrentHealth = health;
	MaxHealth = health;
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
FGridRangeExport* ABattleEntity::GetActionInfo(EActionType type, int32 indexInList)
{
	if (type == EActionType::Attack)
		return &AttackInfoList[indexInList];
	if (type == EActionType::Movement)
		return &MoveInfoList[indexInList];
	return nullptr;
}
void ABattleEntity::CustomTakeDamage(float damage)
{
	CurrentHealth = CurrentHealth - damage;
	if (CurrentHealth <= 0.0f)
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

TOptional<FGridRangeExport> ABattleEntity::LoadActionInfoFromPath(FString path)
{
	FString jsonData;
	// fetch file data and shove it into "jsonData"
	if (FFileHelper::LoadFileToString(jsonData, *path))
	{
		// Convert JSON to struct
		FGridRangeExport exportData;
		if (FJsonObjectConverter::JsonObjectStringToUStruct(jsonData, &exportData, 0, 0))
		{
			return exportData;
		}
	}
	return TOptional<FGridRangeExport>();
}





