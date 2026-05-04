#include "Tasks/G2ITriggerBoxForCreatingTaskShootOnGear.h"
#include "G2I.h"
#include "G2ITaskManager.h"

void AG2ITriggerBoxForCreatingTaskShootOnGear::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	CheckAndCreateTask(OtherActor);
}

void AG2ITriggerBoxForCreatingTaskShootOnGear::CheckAndCreateTask(AActor* OtherActor)
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
	TaskManager->CreateTaskShootOnGear(Gear);
	UnbindPossessDelegate();
}
