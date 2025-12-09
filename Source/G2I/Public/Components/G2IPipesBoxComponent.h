
#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "G2IPipesBoxComponent.generated.h"

/**
 * 
 */
UCLASS()
class G2I_API UG2IPipesBoxComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	bool bRecieves = true;
	TObjectPtr<AActor> Owner;
	int32 PointIndex;
};
