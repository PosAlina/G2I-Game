

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IMovingByGearObjectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UG2IMovingByGearObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class G2I_API IG2IMovingByGearObjectInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void OnPushing(float ForceMagnitude);
};
