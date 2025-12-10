#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IInteractionInputInterface.generated.h"

UINTERFACE()
class UG2IInteractionInputInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface includes methods that are called for interact actions from input handlers.
 */
class G2I_API IG2IInteractionInputInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void InteractAction(const FName& Tag);
};
