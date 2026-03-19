#include "BattleLevelLoader.h"

#include "BattleManager.h"
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
	PrimaryActorTick.bCanEverTick = true;
}

void ABattleLevelLoader::BeginPlay()
{
	Super::BeginPlay();
	
	UDefaultInstance* gameInstance = Cast<UDefaultInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	LoadLevel(FPaths::ProjectContentDir() + gameInstance->Map);
}

void ABattleLevelLoader::EditorLoadLevel()
{
	LoadLevel(MapFilePath);
}

void ABattleLevelLoader::LoadLevel(FString levelPath)
{
	FString content;
	if (FFileHelper::LoadFileToString(content, *levelPath))
	{
		FLevelExportStruct levelData;
		if (FJsonObjectConverter::JsonObjectStringToUStruct(content, &levelData, 0, 0))
		{
			ABattleManager* battleManager =  Cast<ABattleManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ABattleManager::StaticClass()));
			if (!battleManager)
				battleManager = GetWorld()->SpawnActor<ABattleManager>(BattleManagerClass);
			FActorSpawnParameters spawnParams;
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			for (int x = 0; x < levelData.GridSize.X; x++)
			{
				for (int y = 0; y < levelData.GridSize.Y; y++)
				{
					FVector spawnPos = FVector(100 * x, 100 * y, 0.0f);				
					ABattleGridCell* newCell = GetWorld()->SpawnActor<ABattleGridCell>(BattleGridClass, spawnPos, FRotator::ZeroRotator, spawnParams);
					newCell->SetFolderPath(FName("GridCells")); 
					if ((x+y) % 2 == 0)
					{
						newCell->MeshComponent->SetMaterial(0, CheckBoardMat1);
					}
					else
					{
						newCell->MeshComponent->SetMaterial(0, CheckBoardMat2);
					}
					battleManager->GridMap.Add(FIntPoint(x,y), newCell);
					newCell->CellSetUp(FIntPoint(x,y), battleManager);
					newCell->OnClickedCellEvent.AddDynamic(battleManager, &ABattleManager::ABattleManager::OnCellSelected);
				}
			}
			
			for (const auto& cellData : levelData.GridCells)
			{
				FVector propSpawnLoc;
				switch (cellData.CellType)
				{
					case ECellBrushMode::Wall:
						if (TObjectPtr<ABattleGridCell>* newCell = battleManager->GridMap.Find(cellData.Position))
						{
							FString stringConvert = FString::FromInt(cellData.IdForCellContent);
							if (FAssetLookUpTable* propInfo = PropDataTable->FindRow<FAssetLookUpTable>(FName(*stringConvert), TEXT("Level Loader")))
							{
								propSpawnLoc = (*newCell)->GetActorLocation() + FVector(0.0f,0.0f, 100.0f);
								GetWorld()->SpawnActor<AActor>(propInfo->ActorBP, propSpawnLoc, FRotator::ZeroRotator, spawnParams);
								(*newCell)->SetWalkable(false);
							}
						}
					break;
					case ECellBrushMode::PlayerSpawn:
						if (battleManager->PlayerRef == nullptr)
						{
							if (TObjectPtr<ABattleGridCell>* newCell = battleManager->GridMap.Find(cellData.Position))
							{
								propSpawnLoc = (*newCell)->GetActorLocation() + FVector(0.0f,0.0f, 100.0f);
								battleManager->PlayerRef = GetWorld()->SpawnActor<ABattlePlayerEntity>(PlayerEntityClass, propSpawnLoc, FRotator::ZeroRotator, spawnParams);
								battleManager->PlayerRef->SetPos(cellData.Position);
								battleManager->PlayerRef->OnBattleStart(battleManager);
								(*newCell)->HouseEntity(battleManager->PlayerRef);
							}
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("More than one player spawn point, Ignoring..."));
						}
					break;
					case ECellBrushMode::EnemySpawn:
						if (TObjectPtr<ABattleGridCell>* newCell = battleManager->GridMap.Find(cellData.Position))
						{
							FString stringConvert = FString::FromInt(cellData.IdForCellContent);
							if (FEnemyInfoStruct* enemyInfo = EnemyDataTable->FindRow<FEnemyInfoStruct>(FName(*stringConvert), TEXT("Level Loader")))
							{
								propSpawnLoc = (*newCell)->GetActorLocation() + FVector(0.0f,0.0f, 100.0f);
								ABattleEnemyEntity* newEnemy = GetWorld()->SpawnActor<ABattleEnemyEntity>(BaseEnemyEntityClass, propSpawnLoc, FRotator::ZeroRotator, spawnParams);
								// [NOTE] Pass data to enemy
								battleManager->EnemyArray.Add(newEnemy);
								newEnemy->OnBattleStart(battleManager, 1, 1);
								newEnemy->SetPos(cellData.Position);
								(*newCell)->HouseEntity(newEnemy);
							}
						}
					break;
					default:
					break;
				}
			}
			battleManager->SetUpBattle();
		}
	}
}
