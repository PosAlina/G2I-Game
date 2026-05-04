#pragma once

#include "CoreMinimal.h"
#include "G2ITasksListItem.generated.h"

/**
 * Object for creation element of tasks list
 */
UCLASS()
class G2I_API UG2ITasksListItem : public UObject
{
	GENERATED_BODY()
	
public:

	FText Description;
	
};
