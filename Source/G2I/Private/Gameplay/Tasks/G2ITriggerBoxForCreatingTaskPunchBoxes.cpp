#include "Tasks/G2ITriggerBoxForCreatingTaskPunchBoxes.h"
#include "G2I.h"
#include "G2ITaskManager.h"

void AG2ITriggerBoxForCreatingTaskPunchBoxes::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	CheckAndCreateTask(OtherActor);
}

void AG2ITriggerBoxForCreatingTaskPunchBoxes::CheckAndCreateTask(AActor* OtherActor)
{
	Super::CheckAndCreateTask(OtherActor);
	
	if (!CheckCreatedTaskConditions(OtherActor))
	{
		return;
	}
	
	if (Boxes.IsEmpty())
	{
		UE_LOG(LogG2I, Log, TEXT("%s: Not boxes for destruct"), *GetName());
		return;
	}
	
	if (!ensure(TaskManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(), *UG2ITaskManager::StaticClass()->GetName());
		return;
	}
	bIsTaskCreated = true;
	TaskManager->CreateTaskPunchBoxes(Boxes, CountShouldDestructActors);
	UnbindPossessDelegate();
}
 