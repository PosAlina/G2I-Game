#include "G2ITaskManager.h"
#include "G2I.h"
#include "G2IDestructibleActorBase.h"
#include "G2IGameInstance.h"
#include "G2IPlayerController.h"
#include "G2IRotatingBySteamGear.h"
#include "G2ITasksNames.h"
#include "G2IUIManager.h"
#include "Tasks/G2ITriggerBoxForCreatingTaskPunchBoxes.h"

void UG2ITaskManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	GameInstance = Cast<UG2IGameInstance>(GetGameInstance());
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IGameInstance::StaticClass()->GetName());
		return;
	}
	StartGameDelegateHandle = GameInstance->OnStartLevelInitDelegate.AddUObject(this, &ThisClass::InitializeInStartGame);
}

void UG2ITaskManager::InitializeInStartGame()
{
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IGameInstance::StaticClass()->GetName());
		return;
	}
	GameInstance->OnStartLevelInitDelegate.Remove(StartGameDelegateHandle);
	
	UIManager = GameInstance->GetSubsystem<UG2IUIManager>();
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	InitializeInStartLevel();

	GameInstance->OnStartLevelInitDelegate.AddUObject(this, &ThisClass::InitializeInStartLevel);
}


void UG2ITaskManager::InitializeInStartLevel()
{
	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World is null in %s"), *GetName());
		return;
	}
	
	PlayerController = Cast<AG2IPlayerController>(World->GetFirstPlayerController());
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*AG2IPlayerController::StaticClass()->GetName());
		return;
	}
}

void UG2ITaskManager::RemoveTaskInViewport(const EG2ITasksNames TaskName) const
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->RemoveTaskInHUD(TaskName);
}

void UG2ITaskManager::AddTaskInViewport(const EG2ITasksNames TaskName) const
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->AddTaskInHUD(TaskName);
}

void UG2ITaskManager::CompleteTaskSwitchCharacter()
{
	FG2ITaskInfo *TaskInfo = ActiveTasks.Find(EG2ITasksNames::SwitchBetweenCharacters);
	if (!ensure(TaskInfo))
	{
		G2I::DebugWarningMessage(GetName() + ": Attempt to complete not created task SwitchCharacter");
		return;
	}

	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*AG2IPlayerController::StaticClass()->GetName());
		return;
	}
	for (const FDelegateHandle& DelegateHandle : TaskInfo->OnTaskCompletedOutsideDelegates)
	{
		PlayerController->OnSwitchBetweenCharacterDelegate.Remove(DelegateHandle);
	}
	
	RemoveTaskInViewport(EG2ITasksNames::SwitchBetweenCharacters);
	CreateTaskFly();
}

void UG2ITaskManager::CreateTaskSwitchCharacter()
{
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*AG2IPlayerController::StaticClass()->GetName());
		return;
	}
	
	FG2ITaskInfo& NewTaskInfo = ActiveTasks.Add(EG2ITasksNames::SwitchBetweenCharacters);
	NewTaskInfo.OnTaskCompletedOutsideDelegates.Add(
		PlayerController->OnSwitchBetweenCharacterDelegate.AddUObject(
			this, &UG2ITaskManager::CompleteTaskSwitchCharacter));
	
	AddTaskInViewport(EG2ITasksNames::SwitchBetweenCharacters);
}

void UG2ITaskManager::CompleteTaskShootOnGear(AG2IRotatingBySteamGear* Gear)
{
	FG2ITaskInfo *TaskInfo = ActiveTasks.Find(EG2ITasksNames::ShootOnGear);
	if (!ensure(TaskInfo))
	{
		G2I::DebugWarningMessage(GetName() + ": Attempt to complete not created task ShootOnGear");
		return;
	}

	if (!ensure(Gear))
	{
		G2I::DebugWarningMessage(GetName() + ": Attempt to complete task ShootOnGear with null gear");
		return;
	}
	for (const FDelegateHandle& DelegateHandle : TaskInfo->OnTaskCompletedOutsideDelegates)
	{
		Gear->OnStartRotateDelegate.Remove(DelegateHandle);
	}
	
	RemoveTaskInViewport(EG2ITasksNames::ShootOnGear);
}

void UG2ITaskManager::CreateTaskShootOnGear(AG2IRotatingBySteamGear * Gear)
{
	if (!ensure(Gear))
	{
		G2I::DebugWarningMessage(GetName() + ": Attempt to create task ShootOnGear with null gear");
		return;
	}
	FG2ITaskInfo& NewTaskInfo = ActiveTasks.Add(EG2ITasksNames::ShootOnGear);
	NewTaskInfo.OnTaskCompletedOutsideDelegates.Add(
		Gear->OnStartRotateDelegate.AddUObject(this, &UG2ITaskManager::CompleteTaskShootOnGear));
	
	AddTaskInViewport(EG2ITasksNames::ShootOnGear);
}

void UG2ITaskManager::CompleteTaskPunchBox()
{
	--CountShouldDestroyBoxes;
	if (CountShouldDestroyBoxes == 0)
	{
		RemoveTaskInViewport(EG2ITasksNames::PunchBoxes);
	}
}

void UG2ITaskManager::CreateTaskPunchBoxes(
	const TSet<TObjectPtr<AG2IDestructibleActorBase>>& Boxes, const EG2ICountShouldDestructActors CountShouldDestructActors)
{
	if (!ensure(!Boxes.IsEmpty()))
	{
		G2I::DebugWarningMessage(GetName() + ": Attempt to create task PunchBoxes with empty set of boxes");
		return;
	}
	FG2ITaskInfo& NewTaskInfo = ActiveTasks.Add(EG2ITasksNames::PunchBoxes);
	int32 InCountShouldDestructBoxes = 0;
	for (AG2IDestructibleActorBase* Box : Boxes)
	{
		++InCountShouldDestructBoxes;
		NewTaskInfo.OnTaskCompletedOutsideDelegates.Add(
			Box->OnDestroyedDelegate.AddUObject(this, &UG2ITaskManager::CompleteTaskPunchBox));
	}
	
	if (CountShouldDestructActors == EG2ICountShouldDestructActors::AllActors)
	{
		CountShouldDestroyBoxes = InCountShouldDestructBoxes;
	}
	else
	{
		CountShouldDestroyBoxes = 1;
	}
	AddTaskInViewport(EG2ITasksNames::PunchBoxes);
}

void UG2ITaskManager::CompleteTaskFly()
{
	FG2ITaskInfo *TaskInfo = ActiveTasks.Find(EG2ITasksNames::FlyUp);
	if (!ensure(TaskInfo))
	{
		G2I::DebugWarningMessage(GetName() + ": Attempt to complete not created task FlyUp");
		return;
	}

	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*AG2IPlayerController::StaticClass()->GetName());
		return;
	}
	for (const FDelegateHandle& DelegateHandle : TaskInfo->OnTaskCompletedOutsideDelegates)
	{
		PlayerController->OnFlyUpDelegate.Remove(DelegateHandle);
	}
	
	RemoveTaskInViewport(EG2ITasksNames::FlyUp);
}

void UG2ITaskManager::CreateTaskFly()
{
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*AG2IPlayerController::StaticClass()->GetName());
		return;
	}
	
	FG2ITaskInfo& NewTaskInfo = ActiveTasks.Add(EG2ITasksNames::FlyUp);
	NewTaskInfo.OnTaskCompletedOutsideDelegates.Add(
		PlayerController->OnFlyUpDelegate.AddUObject(this, &UG2ITaskManager::CompleteTaskFly));
	
	AddTaskInViewport(EG2ITasksNames::FlyUp);
}
