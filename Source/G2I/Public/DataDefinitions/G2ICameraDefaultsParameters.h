#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "G2ICameraDefaultsParameters.generated.h"

/**
 * Default parameters for all cameras
 */
UCLASS(Blueprintable)
class G2I_API UG2ICameraDefaultsParameters : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Units="s"))
	float CameraTransitionTime = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsInvertedCameraHorizontalRotation = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsInvertedCameraVerticalRotation = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DefaultTargetArm = 220.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DefaultHorizontalOffset = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DefaultVerticalOffset = 70.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimTargetArm = 160.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimHorizontalOffset = 80.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimVerticalOffset = 70.f;
};
