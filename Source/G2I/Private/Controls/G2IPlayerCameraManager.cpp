#include "G2IPlayerCameraManager.h"

void AG2IPlayerCameraManager::SetViewTarget(class AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams)
{
	if (NewViewTarget)
	{
		Super::SetViewTarget(NewViewTarget, TransitionParams);
	}
}

void AG2IPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	Super::UpdateCamera(DeltaTime);
	
	if (GetCameraCacheView().Location != GetLastFrameCameraCacheView().Location)
	{
		OnChangeCameraLocationDelegate.Broadcast(GetCameraCacheView().Location);
	}
}