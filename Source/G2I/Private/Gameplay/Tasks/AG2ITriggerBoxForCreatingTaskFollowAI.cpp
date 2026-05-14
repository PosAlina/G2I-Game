#include "Tasks/AG2ITriggerBoxForCreatingTaskFollowAI.h"
#include "G2I.h"
#include "G2ITaskManager.h"

void AAG2ITriggerBoxForCreatingTaskFollowAI::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	CheckAndCreateTask(OtherActor);
}

void AAG2ITriggerBoxForCreatingTaskFollowAI::CheckAndCreateTask(AActor* OtherActor)
{
	Super::CheckAndCreateTask(OtherActor);
	
	if (!CheckCreatedTaskConditions(OtherActor))
	{
		return;
	}
	
	if (!ensure(TaskManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(), *UG2ITaskManager::StaticClass()->GetName());
		return;
	}
	bIsTaskCreated = true;
	TaskManager->CreateTaskFollowAI();
	UnbindPossessDelegate();
}
