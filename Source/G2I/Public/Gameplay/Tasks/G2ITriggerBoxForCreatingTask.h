#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "G2ITriggerBoxForCreatingTask.generated.h"

class UG2ITaskManager;
class AG2IPlayerController;
enum class EG2ITasksNames : uint8;

UCLASS()
class G2I_API AG2ITriggerBoxForCreatingTask : public ATriggerBox
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, Category = Conditions)
	TSet<TObjectPtr<AActor>> BanCharacters;
	
protected:
	
	UPROPERTY()
	TObjectPtr<AG2IPlayerController> PlayerController;
	
	UPROPERTY()
	TObjectPtr<UG2ITaskManager> TaskManager;
	
	bool bIsTaskCreated = false;
	
	FDelegateHandle OnPossessNewPawnDelegateHandle;

protected:
	
	virtual void BeginPlay() override;
	
	bool CheckCreatedTaskConditions(AActor* OtherActor) const;
	
	virtual void CheckAndCreateTask(AActor *OtherActor) {}
	
	void UnbindPossessDelegate();
	
private:
	
	void SetupDefaults();
	
	void CheckTaskForAllOverlappingActors();
	
	UFUNCTION()
	void CheckTaskForNewPlayerPawn(APawn *Pawn);
	
};
