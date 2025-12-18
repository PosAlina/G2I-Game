#include "G2IPlayerCameraManager.h"

void AG2IPlayerCameraManager::SetViewTarget(class AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams)
{
	if (NewViewTarget)
	{
		Super::SetViewTarget(NewViewTarget, TransitionParams);
	}
}
