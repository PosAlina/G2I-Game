#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IThirdPersonCameraInputInterface.generated.h"

UINTERFACE()
class UG2IThirdPersonCameraInputInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface includes methods that are called for third person camera actions from input handlers
 */
class G2I_API IG2IThirdPersonCameraInputInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void LookAction(const float Yaw);

};
