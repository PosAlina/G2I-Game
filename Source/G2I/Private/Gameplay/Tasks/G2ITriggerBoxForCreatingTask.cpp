#include "Tasks/G2ITriggerBoxForCreatingTask.h"
#include "G2I.h"
#include "G2IGameInstance.h"
#include "G2IPlayerController.h"
#include "G2ITaskManager.h"

void AG2ITriggerBoxForCreatingTask::BeginPlay()
{
	Super::BeginPlay();

	SetupDefaults();
	CheckTaskForAllOverlappingActors();
}

bool AG2ITriggerBoxForCreatingTask::CheckCreatedTaskConditions(AActor* OtherActor) const
{
	if (bIsTaskCreated)
	{
		return false;
	}
	
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find  %s"), *GetActorNameOrLabel(),
			*AG2IPlayerController::StaticClass()->GetName());
		return false;
	}
	if (PlayerController->GetPawn() != OtherActor)
	{
		return false;
	}
	if (BanCharacters.Contains(OtherActor))
	{
		return false;
	}
	return true;
}

void AG2ITriggerBoxForCreatingTask::CheckTaskForNewPlayerPawn(APawn* Pawn)
{
	CheckTaskForAllOverlappingActors();
}

void AG2ITriggerBoxForCreatingTask::UnbindPossessDelegate()
{
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find  %s"), *GetActorNameOrLabel(),
			*AG2IPlayerController::StaticClass()->GetName());
		return;
	}
	PlayerController->OnPossessPawnDelegate.RemoveDynamic(
		this, &AG2ITriggerBoxForCreatingTask::CheckTaskForNewPlayerPawn);
}

void AG2ITriggerBoxForCreatingTask::CheckTaskForAllOverlappingActors()
{
	TArray<AActor*> OverlapActors;
	GetOverlappingActors(OverlapActors);
	for (AActor* Actor : OverlapActors)
	{
		if (!Actor)
		{
			continue;
		}
		CheckAndCreateTask(Actor);
	}
}

void AG2ITriggerBoxForCreatingTask::SetupDefaults()
{
	const UG2IGameInstance *GameInstance = Cast<UG2IGameInstance>(GetGameInstance());
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IGameInstance::StaticClass()->GetName());
	}
	else
	{
		TaskManager = GameInstance->GetSubsystem<UG2ITaskManager>();
		if (!ensure(TaskManager))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(), *UG2ITaskManager::StaticClass()->GetName());
		}
	}
	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(), *UWorld::StaticClass()->GetName());
		return;
	}
	PlayerController = Cast<AG2IPlayerController>(World->GetFirstPlayerController());
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find  %s"), *GetActorNameOrLabel(),
			*AG2IPlayerController::StaticClass()->GetName());
		return;
	}
	PlayerController->OnPossessPawnDelegate.AddDynamic(this, &ThisClass::CheckTaskForNewPlayerPawn);
}
