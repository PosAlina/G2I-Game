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
	
};
