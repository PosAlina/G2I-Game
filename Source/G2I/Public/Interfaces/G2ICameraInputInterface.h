#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2ICameraInputInterface.generated.h"

UINTERFACE()
class UG2ICameraInputInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface includes methods that are called for camera actions from input handlers.
 */
class G2I_API IG2ICameraInputInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void LookAction(const float Yaw, const float Pitch);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void MouseLookAction(const float Yaw, const float Pitch);
};
