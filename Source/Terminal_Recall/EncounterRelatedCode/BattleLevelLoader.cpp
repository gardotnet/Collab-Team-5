#include "BattleLevelLoader.h"

#include "BattleManager.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "JsonObjectConverter.h"
#include "Kismet/GameplayStatics.h"
#include "Terminal_Recall/EntityClasses/BattleEnemyEntity.h"
#include "Terminal_Recall/EntityClasses/BattlePlayerEntity.h"
#include "Terminal_Recall/GamemodesAndOtherRelatedEssential/DefaultInstance.h"
#include "Terminal_Recall/GridCellClasses/BattleGridCell.h"
#include "Terminal_Recall/StructsAndEnums/LevelExportStruct.h"
#include "Terminal_Recall/StructsAndEnums/ForDataTable/AssetLookUpStruct.h"
#include "Terminal_Recall/StructsAndEnums/ForDataTable/EnemyInfoStruct.h"

ABattleLevelLoader::ABattleLevelLoader()
{
	PrimaryActorTick.bCanEverTick = false;
}
void ABattleLevelLoader::BeginPlay()
{
	Super::BeginPlay();
	for (auto& It : TempGridMap)
	{
		if (It.Value)
		{
			It.Value->Destroy();
		}
	}
	TempGridMap.Empty();
	for (AActor* Actor : TempActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	TempActors.Empty();
	// get level to load
	UE_LOG(LogTemp, Warning, TEXT("%s"), *OverwriteLevelPath);
	if (OverwriteLevelPath != "")
		LoadLevel(OverwriteLevelPath);
	else
	{
		UDefaultInstance* gameInstance = Cast<UDefaultInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		LoadLevel(FPaths::ProjectContentDir() + gameInstance->Map);
	}
}

void ABattleLevelLoader::LoadLevel(FString levelPath)
{
	// load the json file 
	FString content;
	if (FFileHelper::LoadFileToString(content, *levelPath))
	{
		FLevelExportStruct levelData;
		if (FJsonObjectConverter::JsonObjectStringToUStruct(content, &levelData, 0, 0))
		{
			// battle manager for game
			ABattleManager* battleManager = Cast<ABattleManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ABattleManager::StaticClass()));
			if (!IsValid(battleManager))
				battleManager = GetWorld()->SpawnActor<ABattleManager>(BattleManagerClass);
			FActorSpawnParameters spawnParams;
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			// create grid
			for (int x = 0; x < levelData.GridSize.X; x++)
			{
				for (int y = 0; y < levelData.GridSize.Y; y++)
				{
					FVector spawnPos = FVector(-100 * x, 100 * y, 0.0f);
					ABattleGridCell* newCell = GetWorld()->SpawnActor<ABattleGridCell>(BattleGridClass, spawnPos, FRotator::ZeroRotator, spawnParams);
					#if WITH_EDITOR
					newCell->SetFolderPath(FName("GridCells"));
					#endif
					// check board pattern
					if ((x+y) % 2 == 0)
					{
						newCell->MeshComponent->SetMaterial(0, CheckBoardMat1);
					}
					else
					{
						newCell->MeshComponent->SetMaterial(0, CheckBoardMat2);
					}
					// give battle manager a ref to the grid cell and set it up for battle
					battleManager->GridMap.Add(FIntPoint(x,y), newCell);
					newCell->CellSetUp(FIntPoint(x,y), battleManager);
					newCell->OnClickedCellEvent.AddDynamic(battleManager, &ABattleManager::ABattleManager::OnCellSelected);
				}
			}
			// Assign stuff to the cells if they need it 
			for (const auto& cellData : levelData.GridCells)
			{
				FVector propSpawnLoc;
				switch (cellData.CellType)
				{
					// if it is a prop
					case ECellBrushMode::Wall:
						if (TObjectPtr<ABattleGridCell>* newCell = battleManager->GridMap.Find(cellData.Position))
						{
							if (cellData.IdForCellContent == 0)
							{
								(*newCell)->Destroy();
								battleManager->GridMap.Remove(cellData.Position);
								continue;
							}
							FString stringConvert = FString::FromInt(cellData.IdForCellContent);
							// look for prop info using ID given
							if (FAssetLookUpTable* propInfo = PropDataTable->FindRow<FAssetLookUpTable>(FName(*stringConvert), TEXT("Level Loader")))
							{
								propSpawnLoc = (*newCell)->GetActorLocation() + FVector(0.0f,0.0f, 100.0f);
								GetWorld()->SpawnActor<AActor>(propInfo->ActorBP, propSpawnLoc, FRotator::ZeroRotator, spawnParams);
								(*newCell)->SetWalkable(false);
							}
						}
					break;
					// if it is player spawn
					case ECellBrushMode::PlayerSpawn:
						// Check if player is already spawned
						if (!IsValid(battleManager->PlayerRef))
						{
							if (TObjectPtr<ABattleGridCell>* newCell = battleManager->GridMap.Find(cellData.Position))
							{
								propSpawnLoc = (*newCell)->GetActorLocation() + FVector(0.0f,0.0f, 100.0f);
								battleManager->PlayerRef = GetWorld()->SpawnActor<ABattlePlayerEntity>(PlayerEntityClass, propSpawnLoc, FRotator::ZeroRotator, spawnParams);
								battleManager->PlayerRef->SetPos(cellData.Position, *newCell);
								battleManager->PlayerRef->OnBattleStart(battleManager);
								battleManager->PlayerRef->OnActionDoneEvent.AddDynamic(battleManager, &ABattleManager::NextEntityTurn);
								(*newCell)->HouseEntity(battleManager->PlayerRef);
							}
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("More than one player spawn point, Ignoring..."));
						}
					break;
					// if it is enemy spawn
					case ECellBrushMode::EnemySpawn:
						if (TObjectPtr<ABattleGridCell>* newCell = battleManager->GridMap.Find(cellData.Position))
						{
							propSpawnLoc = (*newCell)->GetActorLocation() + FVector(0.0f,0.0f, 100.0f);
							ABattleEnemyEntity* newEnemy = GetWorld()->SpawnActor<ABattleEnemyEntity>(BaseEnemyEntityClass, propSpawnLoc, FRotator::ZeroRotator, spawnParams);
							// [NOTE] Pass data to enemy
							battleManager->EnemyArray.Add(newEnemy);
							newEnemy->OnActionDoneEvent.AddDynamic(battleManager, &ABattleManager::NextEntityTurn);
							newEnemy->OnBattleStart(battleManager, cellData.IdForCellContent);
							newEnemy->SetPos(cellData.Position, *newCell);
							(*newCell)->HouseEntity(newEnemy);
						}
					break;
					default:
					break;
				}
			}
		}
	}
}
// same thing as load level but not setting up for battle
void ABattleLevelLoader::EditorLoadLevel()
{
	for (auto& It : TempGridMap)
	{
		if (It.Value)
		{
			It.Value->Destroy();
		}
	}
	TempGridMap.Empty();
	for (AActor* Actor : TempActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	TempActors.Empty();
	UWorld* world = GetWorld();
	if (IsValid(world))
	{
		UE_LOG(LogTemp, Warning, TEXT("Editor Load Level"));
		world->SetFlags(RF_Transient);
	}
	
	// set up to grab file for the file explorer
	if (IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get())
	{
		// Basic setup json file search
		TArray<FString> OutFileNames;
		bool bOpened = DesktopPlatform->OpenFileDialog(
			nullptr, 
			TEXT("Select a File"), 
			TEXT("C:/"), 
			TEXT(""), 
			TEXT("JSON Files (*.json)|*.json|All Files (*.*)|*.*"), 
			EFileDialogFlags::None, 
			OutFileNames
		);
		// if selected a file
		if (bOpened && OutFileNames.Num() > 0)
		{
			OverwriteLevelPath = OutFileNames[0];
			FString jsonData;
			// fetch file data and shove it into "jsonData"
			if (FFileHelper::LoadFileToString(jsonData, *OverwriteLevelPath))
			{
				// Convert JSON to struct
				FLevelExportStruct levelData;
				if (FJsonObjectConverter::JsonObjectStringToUStruct(jsonData, &levelData, 0, 0))
				{
					// The modified load function 
					AActor* player = nullptr; 
					FActorSpawnParameters spawnParams;
					spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					for (int x = 0; x < levelData.GridSize.X; x++)
					{
						for (int y = 0; y < levelData.GridSize.Y; y++)
						{
							FVector spawnPos = FVector(100 * x, 100 * y, 0.0f) + FVector(50.0f, 10550.0f, 0.0f);				
							ABattleGridCell* newCell = GetWorld()->SpawnActor<ABattleGridCell>(BattleGridClass, spawnPos, FRotator::ZeroRotator, spawnParams);
							newCell->SetFlags(RF_Transient);
							#if WITH_EDITOR
							newCell->SetLockLocation(true);
							newCell->SetFolderPath(FName("GridCells"));
							#endif
							if ((x+y) % 2 == 0)
							{
								newCell->MeshComponent->SetMaterial(0, CheckBoardMat1);
							}
							else
							{
								newCell->MeshComponent->SetMaterial(0, CheckBoardMat2);
							}
							TempGridMap.Add(FIntPoint(y,x), newCell);
						}
					}
					for (const auto& cellData : levelData.GridCells)
					{
						FVector propSpawnLoc;
						switch (cellData.CellType)
						{
						case ECellBrushMode::Wall:
							if (TObjectPtr<ABattleGridCell>* newCell = TempGridMap.Find(cellData.Position))
							{
								FString stringConvert = FString::FromInt(cellData.IdForCellContent);
								if (FAssetLookUpTable* propInfo = PropDataTable->FindRow<FAssetLookUpTable>(FName(*stringConvert), TEXT("Level Loader")))
								{
									propSpawnLoc = (*newCell)->GetActorLocation() + FVector(0.0f,0.0f, 100.0f);
									AActor* wall = GetWorld()->SpawnActor<AActor>(propInfo->ActorBP, propSpawnLoc, FRotator::ZeroRotator, spawnParams);
									wall->SetFlags(RF_Transient);
									#if WITH_EDITOR
									wall->SetLockLocation(true);
									wall->SetFolderPath(FName("Stuff from battle level"));
									#endif
									TempActors.Add(wall);
								}
							}
							break;
						case ECellBrushMode::PlayerSpawn:
							if (player == nullptr)
							{
								if (TObjectPtr<ABattleGridCell>* newCell = TempGridMap.Find(cellData.Position))
								{
									propSpawnLoc = (*newCell)->GetActorLocation() + FVector(0.0f,0.0f, 100.0f);
									player = GetWorld()->SpawnActor<ABattlePlayerEntity>(PlayerEntityClass, propSpawnLoc, FRotator::ZeroRotator, spawnParams);
									player->SetFlags(RF_Transient);
									#if WITH_EDITOR
									player->SetLockLocation(true);
									player->SetFolderPath(FName("Stuff from battle level"));
									#endif
									TempActors.Add(player);
								}
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("More than one player spawn point, Ignoring..."));
							}
							break;
						case ECellBrushMode::EnemySpawn:
							if (TObjectPtr<ABattleGridCell>* newCell = TempGridMap.Find(cellData.Position))
							{
								FString stringConvert = FString::FromInt(cellData.IdForCellContent);
								propSpawnLoc = (*newCell)->GetActorLocation() + FVector(0.0f,0.0f, 100.0f);
								AActor* enemyChar = GetWorld()->SpawnActor<ABattleEnemyEntity>(BaseEnemyEntityClass, propSpawnLoc, FRotator::ZeroRotator, spawnParams);
								enemyChar->SetFlags(RF_Transient);
								#if WITH_EDITOR
								enemyChar->SetLockLocation(true);
								enemyChar->SetFolderPath(FName("Stuff from battle level"));
								#endif
								TempActors.Add(enemyChar);
							}
							break;
						default:
							break;
						}
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Unable to load \"%s\""), *OverwriteLevelPath)
				OverwriteLevelPath = "";
			}
		}
	}
}