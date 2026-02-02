#pragma once

#include "CoreMinimal.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "G2INavigationFilterBlockPassableObjects.generated.h"

/**
 * Navigation's rules for character (engineer) that cannot pass through special passable objects
 */
UCLASS()
class G2I_API UG2INavigationFilterBlockPassableObjects : public UNavigationQueryFilter
{
	GENERATED_BODY()

public:
	
	UG2INavigationFilterBlockPassableObjects();
	
};
