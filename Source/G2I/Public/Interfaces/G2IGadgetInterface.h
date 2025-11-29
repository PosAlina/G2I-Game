

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IGadgetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UG2IGadgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class G2I_API IG2IGadgetInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void GadgetActivation();
};
