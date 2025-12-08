#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2ICameraInterface.generated.h"

class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAddCameraDelegate, UCameraComponent *, CameraComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRemoveCameraDelegate, UCameraComponent *, CameraComponent);

UINTERFACE()
class UG2ICameraInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface determines common camera type
 */
class G2I_API IG2ICameraInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetupCameras();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<UCameraComponent *> GetCameraComponents() const;

	UFUNCTION()
	virtual FAddCameraDelegate& GetAddCameraDelegate() = 0;

	UFUNCTION()
	virtual FRemoveCameraDelegate& GetRemoveCameraDelegate() = 0;
	
};
