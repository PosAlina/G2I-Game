#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "UObject/Interface.h"
#include "G2ICameraControllerInputInterface.generated.h"

UINTERFACE()
class UG2ICameraControllerInputInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface includes methods that are switch between different camera's behaviors
 */
class G2I_API IG2ICameraControllerInputInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetupCurrentCamera();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SwitchCameraBehavior();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UCameraComponent *GetCameraComponent() const;
	
};
