#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2ITasksScreen.generated.h"

class UG2ITasksListItem;
enum class EG2ITasksNames : uint8;
class UListView;

/**
 * Screen with tasks of player
 */
UCLASS()
class G2I_API UG2ITasksScreen : public UG2IUserWidget
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UListView> TasksList;
	
private:
	
	UPROPERTY()
	TMap<EG2ITasksNames, TObjectPtr<UG2ITasksListItem>> ActualTasks;
	
public:
	
	void SetRow(const FText& Description, EG2ITasksNames TaskName);
	
	void RemoveRow(EG2ITasksNames TaskName);

};
