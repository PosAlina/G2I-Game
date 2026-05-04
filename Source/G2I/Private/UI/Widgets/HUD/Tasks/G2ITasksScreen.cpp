#include "HUD/Tasks/G2ITasksScreen.h"
#include "G2I.h"
#include "Components/ListView.h"
#include "HUD/Tasks/G2ITasksListItem.h"

void UG2ITasksScreen::SetRow(const FText& Description, const EG2ITasksNames TaskName)
{
	if (!ensure(TasksList))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find ListView in TasksList"), *GetName());
		return;
	}
	const TObjectPtr<UG2ITasksListItem> Item = NewObject<UG2ITasksListItem>();
	if (!ensure(Item))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: TasksListItem couldn't created"), *GetName());
		return;
	}
	Item->Description = Description;
	TasksList->AddItem(Item);
	ActualTasks.Add(TaskName, Item);
}

void UG2ITasksScreen::RemoveRow(const EG2ITasksNames TaskName)
{
	const TObjectPtr<UG2ITasksListItem> *ItemPtr = ActualTasks.Find(TaskName);
	if (!ensure(ItemPtr))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Attempt to remove unexist task row"), *GetName());
	}
	TasksList->RemoveItem(*ItemPtr);
	ActualTasks.Remove(TaskName);
}
