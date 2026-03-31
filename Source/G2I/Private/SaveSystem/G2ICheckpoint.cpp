#include "G2ICheckpoint.h"
#include "G2I.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AG2ICheckpoint::AG2ICheckpoint()
{
	UBoxComponent* CollisionBoxComponent = Cast<UBoxComponent>(GetCollisionComponent());

	if (CollisionBoxComponent)
	{
		CollisionBoxComponent->SetGenerateOverlapEvents(true);
		CollisionBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		CollisionBoxComponent->SetCollisionObjectType(ECC_WorldStatic);
		CollisionBoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		// Also enabling overlaps for passing through actor
		CollisionBoxComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
		CollisionBoxComponent->SetBoxExtent(FVector(400.f, 400.f, 400.f));
	}
}

void AG2ICheckpoint::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITOR
	SetActorHiddenInGame(false);
#endif

	const UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogG2I, Error, TEXT("GameInstance is NULL in %s."), *GameInstance->GetName(), *GetName());
		return;
	}

	SavingGameplayManager = GameInstance->GetSubsystem<UG2ISavingGameplayManager>();
	if (!ensure(SavingGameplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("Couldn't get SavingGameplayManager subsystem from GameInstance in %s."), *GetName());
		return;
	}

	SavingGameplayManager->LoadRequestedData(this);
	SavingGameplayManager->OnGameplaySavedDelegate.AddUniqueDynamic(this, &ThisClass::OnGameplaySaved);
}

void AG2ICheckpoint::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	const ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor);
	if (!OtherCharacter)
	{
		return;
	}
	if (!OtherCharacter->IsPlayerControlled())
	{
		UE_LOG(LogG2I, Log, TEXT("AI controlled character overlapped %s. Saving isn't triggered."), *GetName());
		return;
	}

	bActivated = true;

	if (!SavingGameplayManager)
	{
		UE_LOG(LogG2I, Error, TEXT("SavingGameplayManager is null in %s."), *GetName());
		return;
	}
	
	SavingGameplayManager->SaveAllDataAndGameplay(true);
}

void AG2ICheckpoint::OnGameplaySaved(bool bSuccess)
{
	if (bActivated)
	{
		Destroy();
	}
}

void AG2ICheckpoint::SaveData_Implementation(UG2IGameplaySaveGame* SaveGameRef)
{
	if (!ensure(SaveGameRef))
	{
		UE_LOG(LogG2I, Warning, TEXT("Got null SaveGameRef while trying to save %s's data."), *GetName());
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogG2I, Error, TEXT("World is null in %s."), *GetName());
		return;
	}

	const FCheckpointSaveData CheckpointSavedData = FCheckpointSaveData(UGameplayStatics::GetCurrentLevelName(World), GetActorLocation());
	SaveGameRef->CheckpointsSaveData.Add(CheckpointSavedData, bActivated);
}

void AG2ICheckpoint::LoadData_Implementation(const UG2IGameplaySaveGame* SaveGameRef)
{
	if (!ensure(SaveGameRef))
	{
		UE_LOG(LogG2I, Warning, TEXT("Got null SaveGameRef while trying to load %s's data."), *GetName());
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogG2I, Error, TEXT("World is null in %s."), *GetName());
		return;
	}

	const FCheckpointSaveData CheckpointData = FCheckpointSaveData(UGameplayStatics::GetCurrentLevelName(World), GetActorLocation());
	if (const bool* Key = SaveGameRef->CheckpointsSaveData.Find(CheckpointData))
	{
		if (*Key) { Destroy(); }
	}
}
