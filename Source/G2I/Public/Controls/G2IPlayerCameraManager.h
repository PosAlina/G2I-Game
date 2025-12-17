#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "G2IPlayerCameraManager.generated.h"

/**
 * Basic Player Camera Manager
 * Doesn't switch on Player Controller when trying setting empty camera
 */
UCLASS()
class G2I_API AG2IPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	virtual void SetViewTarget(AActor* NewViewTarget,
		FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams()) override;
	
};
