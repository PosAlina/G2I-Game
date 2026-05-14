#pragma once

#include "CoreMinimal.h"
#include "Tasks/G2ITriggerBoxForCreatingTask.h"
#include "G2ITaskManager.generated.h"

enum class EG2ICountShouldDestructActors : uint8;
class AG2IDestructibleActorBase;
class AG2IRotatingBySteamGear;
class AG2IPlayerController;
class UG2IUIManager;
class UG2IGameInstance;
enum class EG2ITasksNames : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTaskManagerInitialized);

USTRUCT(BlueprintType)
struct FG2ITaskInfo
{
	GENERATED_BODY()
	
	TSet<FDelegateHandle> OnTaskCompletedOutsideDelegates;
};

UCLASS()
class G2I_API UG2ITaskManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	
	UPROPERTY()
	TObjectPtr<UG2IGameInstance> GameInstance;
	
	UPROPERTY()
	TObjectPtr<UG2IUIManager> UIManager;
	
	UPROPERTY()
	TObjectPtr<AG2IPlayerController> PlayerController;
	
	UPROPERTY()
	TMap<EG2ITasksNames, FG2ITaskInfo> ActiveTasks;
	
	FDelegateHandle StartGameDelegateHandle;
	
	int32 CountShouldDestroyBoxes = 1;
public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION()
	void CompleteTaskSwitchCharacter();
	void CreateTaskSwitchCharacter();
	
	UFUNCTION()
	void CompleteTaskShootOnGear(AG2IRotatingBySteamGear* Gear);
	void CreateTaskShootOnGear(AG2IRotatingBySteamGear * Gear);
	
	UFUNCTION()
	void CompleteTaskPunchBox();
	void CreateTaskPunchBoxes(const TSet<TObjectPtr<AG2IDestructibleActorBase>>& Boxes,
		EG2ICountShouldDestructActors CountShouldDestructActors);
	
	UFUNCTION()
	void CompleteTaskFly();
	void CreateTaskFly();
	
	UFUNCTION()
	void CompleteTaskFollowAI(bool bCanMove);
	void CreateTaskFollowAI();

private:
	
	UFUNCTION()
	void InitializeInStartGame();
	
	UFUNCTION()
	void InitializeInStartLevel();
	
	void RemoveTaskInViewport(EG2ITasksNames TaskName) const;
	void AddTaskInViewport(EG2ITasksNames TaskName) const;
	
};