
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IActivationInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UG2IActivationInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for objects that need to be activated / deactivated.
 */
class G2I_API IG2IActivationInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Activation")
	void Activate();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Activation")
	void Deactivate();
};
