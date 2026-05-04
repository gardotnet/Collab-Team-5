#include "BattleEntity.h"

#include "JsonObjectConverter.h"
#include "Components/TextBlock.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/EncounterRelatedCode/BattleManager.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/Controllers/PlayerBattleController.h"
#include "Terminal_Recall/StructsAndEnums/ForDataTable/ActionStruct.h"
#include "Terminal_Recall/StuffForUI/ActionButtonUI.h"
#include "Terminal_Recall/StatusEffect/BaseStatusEffect.h"
#include "Terminal_Recall/StuffForUI/BattleChoiceSelectUI.h"

ABattleEntity::ABattleEntity()
{
	PrimaryActorTick.bCanEverTick = true;
	TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>("Text Component");
	TextRenderComponent->SetupAttachment(RootComponent);
	TextRenderComponent->AddLocalOffset(FVector(0.0f, 0.0f, 100.0f));
	TextRenderComponent->SetText(FText::FromString("-1"));
	TextRenderComponent->AddLocalRotation(FRotator(90.0f, 180.0f, 0.0f));
	TextRenderComponent->SetTextRenderColor(FColor::Blue);
	TextRenderComponent->VerticalAlignment = EVerticalTextAligment::EVRTA_TextCenter;
	TextRenderComponent->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
	TextRenderComponent->SetHiddenInGame(true);
	TextRenderComponent->YScale = 2.0f;
	TextRenderComponent->XScale = 2.0f;
}
void ABattleEntity::OnBattleStart(ABattleManager* battleManager)
{
	BattleManager = battleManager;
}

void ABattleEntity::OnBattleStart(ABattleManager* battleManager, int32 enemyID)
{
	BattleManager = battleManager;
}

void ABattleEntity::OnTurnGo()
{
	ShowChoices();
}
void ABattleEntity::ShowChoices()
{
	// add all buttons to the UI
	
	//PlayerBattleController->BattleChoiceUIWidget->AddToViewport();
	//PlayerBattleController->BattleChoiceUIWidget->AddActionButton(MovementActionsUI, AttackActionsUI, ItemActionsUI);
}
void ABattleEntity::SetPos(FIntPoint newPos, ABattleGridCell* cell)
{
	UE_LOG(LogTemp, Display, TEXT("SetPos"));
	OnChangeCell.Broadcast();
	CurrentCell = cell;
	GridPos = newPos;
}
FGridRangeExport* ABattleEntity::GetActionInfo(EActionType type, int32 indexInList)
{
	if (type == EActionType::Attack || type == EActionType::EnemyAttack)
		return &ActionList[indexInList].AttackAction.ActionRange;
	if (type == EActionType::Movement || type == EActionType::EnemyMove)
		return &ActionList[indexInList].MovementAction.ActionRange;
	return nullptr;
}

void ABattleEntity::EnableRestrictedMovement()
{
	bRestrictedMovement = true;
	// [NOTE] cancel UI here
}
void ABattleEntity::DisableRestrictedMovement()
{
	bRestrictedMovement = false;
	// [NOTE] enable UI here
}
void ABattleEntity::CustomTakeDamage(float damage)
{
	CurrentHealth = CurrentHealth - damage;
	OnDamagedEvent.Broadcast(CurrentHealth);
	if (CurrentHealth <= 0.0f)
	{
		// tells the grid it 'WAS' on, that it is now empty
		OnChangeCell.Broadcast();
		Destroy();
		BattleManager->NextEntityTurn();
	}
	else if (CurrentHealth > MaxHealth)
	{
		CurrentHealth = MaxHealth;
	}
}

void ABattleEntity::CustomTakeDamageAndApplyStatus(float damage, ABaseStatusEffect* effect)
{
	CustomTakeDamage(damage);
	CurrentStatusEffects.Add(effect);
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
		OnActionComplete(EActionType::Movement);
		OnActionDoneEvent.Broadcast();
		BattleManager->NextEntityTurn();
	}
	else
	{
		SetActorLocation(FMath::Lerp(playerPos, TargetPosition, MoveTimer/DesiredMoveDuration));
	}
}
void ABattleEntity::OnEntityClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	//BattleManager->OnEntitySelected(this);
	
}

void ABattleEntity::OnActionComplete(EActionType type)
{
	ReduceCooldown();
	AddCooldown(type);
	HandleStatusEffects();
}

void ABattleEntity::AddCooldown(EActionType type)
{
	if (!ActionList.IsValidIndex(SelectedActionID))
		return;
	if (ActionList[SelectedActionID].Cooldown < 0)
		return;
	UE_LOG(LogTemp, Warning, TEXT("ID: %i"), SelectedActionID);
	if (type == EActionType::Movement)
	{
		ActionList[SelectedActionID].Cooldown = ActionList[SelectedActionID].MovementAction.CooldownDuration;
		UE_LOG(LogTemp, Warning, TEXT("move cd: %i"), ActionList[SelectedActionID].Cooldown)
	}
	else if (type == EActionType::Attack)
	{
		ActionList[SelectedActionID].Cooldown = ActionList[SelectedActionID].AttackAction.CooldownDuration;
		UE_LOG(LogTemp, Warning, TEXT("attack cd: %i"), ActionList[SelectedActionID].Cooldown)
	}
	if (ActionList[SelectedActionID].Cooldown == 0)
		return;
	if (AttackActionsUI.IsValidIndex(SelectedActionID))
		AttackActionsUI[SelectedActionID]->SetCooldown(ActionList[SelectedActionID].Cooldown);
	if (MovementActionsUI.IsValidIndex(SelectedActionID))
		MovementActionsUI[SelectedActionID]->SetCooldown(ActionList[SelectedActionID].Cooldown);
}

void ABattleEntity::ReduceCooldown()
{
	for (int i = 0; i < 5; i++)
	{
		if (ActionList[i].Cooldown > 0)
		{
			ActionList[i].Cooldown--;
			if (AttackActionsUI.IsValidIndex(i))
				AttackActionsUI[i]->SetCooldown(ActionList[i].Cooldown);
			if (MovementActionsUI.IsValidIndex(i))
				MovementActionsUI[i]->SetCooldown(ActionList[i].Cooldown);
		}
	}
}

void ABattleEntity::ShowPotentialHit(float damage)
{
	TextRenderComponent->SetHiddenInGame(false);
	TextRenderComponent->SetText(FText::FromString("-" + FString::FromInt(damage)));
}

void ABattleEntity::HidePotentialHit()
{
	TextRenderComponent->SetHiddenInGame(true);
}

// finding action in data table and making a button for it
void ABattleEntity::FindAndSaveActions(const TArray<int32>& attackArray, const TArray<int32>& moveArray, bool isEnemy)
{
	ActionList.SetNum(5);
	EActionType attackType = EActionType::Attack;
	EActionType moveType = EActionType::Movement;
	if (isEnemy)
	{
		attackType = EActionType::EnemyAttack;
		moveType = EActionType::EnemyMove;
	}
	// find attack info and set up button 
	
	for (int i = 0; i < 5; i++)
	{
		if (!attackArray.IsValidIndex(i) || attackArray[i] <= 0)
			continue;
		FString stringConvert = FString::FromInt(attackArray[i]);
		if (FActionStruct* attackInfo = ActionDataTable->FindRow<FActionStruct>(FName(*stringConvert), TEXT("Entity Search")))
		{
			auto extractedData = LoadActionInfoFromPath(FPaths::ProjectContentDir() + attackInfo->GridRangeFilePath);
			if (!extractedData.IsSet())
				continue;
			ActionList[i].AttackAction = FActionRefStruct(
				extractedData.GetValue(), 
				attackInfo->CooldownDuration,
				attackInfo->Damage,
				attackInfo->StatusEffect,
				attackInfo->bIsSelfAppliedStatus
			);
			if (IsValid(attackInfo->StatusEffect))
			{
				UE_LOG(LogTemp, Warning, TEXT("Has Effect"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No effect"));
			}
			ActionList[i].Cooldown = 0;
			// create button and set it up
			if (UActionButtonUI* newAttackButton =Cast<UActionButtonUI>(CreateWidget(GetWorld()->GetFirstPlayerController(), ActionButtonClass)))
			{
				newAttackButton->Entity = this;
				newAttackButton->ActionType = attackType;
				newAttackButton->ActionName = attackInfo->ActionName.ToString();
				newAttackButton->NameLabel->SetText(FText::FromString(attackInfo->ActionName.ToString()));
				AttackActionsUI.Add(newAttackButton);
				newAttackButton->IndexInOwningEntityArray = i;
				newAttackButton->OnActionSelected.AddDynamic(BattleManager, &ABattleManager::ActionChoiceSelected);
			}
		}
	}
	for (int i = 0; i < 5; i++)
	{
		if (!moveArray.IsValidIndex(i) || moveArray[i] <= 0)
			continue;
		FString stringConvert = FString::FromInt(moveArray[i]);
		if (FActionStruct* moveInfo = ActionDataTable->FindRow<FActionStruct>(FName(*stringConvert), TEXT("Entity Search")))
		{
			auto extractedData = LoadActionInfoFromPath(FPaths::ProjectContentDir() + moveInfo->GridRangeFilePath);
			if (!extractedData.IsSet())
				continue;
			ActionList[i].MovementAction = FActionRefStruct(
				extractedData.GetValue(), 
				moveInfo->CooldownDuration
			);
			ActionList[i].Cooldown = 0;
			if (UActionButtonUI* newMoveButton =Cast<UActionButtonUI>(CreateWidget(GetWorld()->GetFirstPlayerController(), ActionButtonClass)))
			{
				newMoveButton->Entity = this;
				newMoveButton->ActionType = moveType;
				newMoveButton->ActionName = moveInfo->ActionName.ToString();
				newMoveButton->NameLabel->SetText(FText::FromString(moveInfo->ActionName.ToString()));
				MovementActionsUI.Add(newMoveButton);
				newMoveButton->IndexInOwningEntityArray = i;
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

void ABattleEntity::HandleStatusEffects()
{
	for (ABaseStatusEffect* status : CurrentStatusEffects)
	{
		status->DoStatusEffect_Implementation(this);
	}
}
