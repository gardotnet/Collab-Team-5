#include "BattleEntity.h"

#include "JsonObjectConverter.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/EncounterRelatedCode/BattleManager.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/Controllers/PlayerBattleController.h"
#include "Terminal_Recall/StructsAndEnums/ForDataTable/ActionStruct.h"
#include "Terminal_Recall/StuffForUI/ActionButtonUI.h"
#include "Terminal_Recall/StuffForUI/BattleChoiceSelectUI.h"

ABattleEntity::ABattleEntity()
{
	PrimaryActorTick.bCanEverTick = true;
}
void ABattleEntity::OnBattleStart(ABattleManager* battleManager)
{
	BattleManager = battleManager;
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
void ABattleEntity::SetPos(FIntPoint newPos, ABattleGridCell* cell)
{
	OnChangeCell.Broadcast();
	CurrentCell = cell;
	GridPos = newPos;
}
FGridRangeExport* ABattleEntity::GetActionInfo(EActionType type, int32 indexInList)
{
	if (type == EActionType::Attack || type == EActionType::EnemyAttack)
		return &AttackInfoList[indexInList].ActionRange;
	if (type == EActionType::Movement || type == EActionType::EnemyMove)
		return &MoveInfoList[indexInList].ActionRange;
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
		TimerManagerRef->ClearTimer(MoveTimerHandle);
		SetActorLocation(TargetPosition);
		OnActionDoneEvent.Broadcast();
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
// finding action in data table and making a button for it
void ABattleEntity::FindAndSaveActions(const TArray<int32>& attackArray, const TArray<int32>& moveArray, bool isEnemy)
{
	EActionType attackType = EActionType::Attack;
	EActionType moveType = EActionType::Movement;
	if (isEnemy)
	{
		attackType = EActionType::EnemyAttack;
		moveType = EActionType::EnemyMove;
	}
	// find attack info and set up button 
	for (auto attackID : attackArray)
	{
		FString stringConvert = FString::FromInt(attackID);
		if (FActionStruct* attackInfo = ActionDataTable->FindRow<FActionStruct>(FName(*stringConvert), TEXT("Entity Search")))
		{
			auto extractedData = LoadActionInfoFromPath(FPaths::ProjectContentDir() + attackInfo->GridRangeFilePath);
			if (!extractedData.IsSet())
				continue;
			AttackInfoList.Add(FMoveStruct(extractedData.GetValue(), 0));
			// create button and set it up
			if (UActionButtonUI* newAttackButton =Cast<UActionButtonUI>(CreateWidget(GetWorld()->GetFirstPlayerController(), ActionButtonClass)))
			{
				newAttackButton->Entity = this;
				newAttackButton->ActionType = attackType;
				newAttackButton->ActionName = attackInfo->ActionName.ToString();
				newAttackButton->NameLabel->SetText(FText::FromString(attackInfo->ActionName.ToString()));
				AttackActionsUI.Add(newAttackButton);
				newAttackButton->IndexInOwningEntityArray = AttackActionsUI.Num() - 1;
				newAttackButton->OnActionSelected.AddDynamic(BattleManager, &ABattleManager::ActionChoiceSelected);
			}
		}
	}
	// find move info and set up button 
	for (auto moveID : moveArray)
	{
		FString stringConvert = FString::FromInt(moveID);
		if (FActionStruct* moveInfo = ActionDataTable->FindRow<FActionStruct>(FName(*stringConvert), TEXT("Entity Search")))
		{
			auto extractedData = LoadActionInfoFromPath(FPaths::ProjectContentDir() + moveInfo->GridRangeFilePath);
			if (!extractedData.IsSet())
				continue;
			MoveInfoList.Add(FMoveStruct(extractedData.GetValue(), 0));
			if (UActionButtonUI* newMoveButton =Cast<UActionButtonUI>(CreateWidget(GetWorld()->GetFirstPlayerController(), ActionButtonClass)))
			{
				newMoveButton->Entity = this;
				newMoveButton->ActionType = moveType;
				newMoveButton->ActionName = moveInfo->ActionName.ToString();
				newMoveButton->NameLabel->SetText(FText::FromString(moveInfo->ActionName.ToString()));
				MovementActionsUI.Add(newMoveButton);
				newMoveButton->IndexInOwningEntityArray = MovementActionsUI.Num() - 1;
				newMoveButton->OnActionSelected.AddDynamic(BattleManager, &ABattleManager::ActionChoiceSelected);
			}
		}
	}
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




