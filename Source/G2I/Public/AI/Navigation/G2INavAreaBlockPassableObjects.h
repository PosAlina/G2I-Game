#pragma once

#include "CoreMinimal.h"
#include "NavAreas/NavArea.h"
#include "G2INavAreaBlockPassableObjects.generated.h"

/**
 * Navigation's area around special objects that can be passed through by a certain character (daughter)
 */
UCLASS()
class G2I_API UG2INavAreaBlockPassableObjects : public UNavArea
{
	GENERATED_BODY()

public:

	UG2INavAreaBlockPassableObjects();
	
};
