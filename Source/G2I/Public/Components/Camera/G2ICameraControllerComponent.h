#pragma once

#include "CoreMinimal.h"
#include "G2ICameraControllerInputInterface.h"
#include "G2ICameraStateEnums.h"
#include "Components/ActorComponent.h"
#include "G2ICameraControllerComponent.generated.h"

class UG2ICameraDefaultsParameters;
class AG2IPlayerController;
class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSetThirdPersonCameraTypeDelegate,
	EG2ICameraBlendState, CurrentBlendState, const UCameraComponent *, NewCamera);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSetFixedCameraTypeDelegate,
	EG2ICameraBlendState, CurrentBlendState, const UCameraComponent *, NewCamera, float, DelayMovementTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FThirdPersonCameraYawRotationDelegate, double, OldCameraYawRotation);

UCLASS(ClassGroup=(Camera), meta=(BlueprintSpawnableComponent))
class G2I_API UG2ICameraControllerComponent : public UActorComponent, public IG2ICameraControllerInputInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FSetThirdPersonCameraTypeDelegate OnSetThirdPersonCameraTypeDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FSetFixedCameraTypeDelegate OnSetFixedCameraTypeDelegate;

	UPROPERTY(BlueprintAssignable)
	FThirdPersonCameraYawRotationDelegate OnThirdPersonCameraYawRotationDelegate;
	
private:

	UPROPERTY()
	TObjectPtr<ACharacter> Owner;
	
	UPROPERTY()
	TObjectPtr<AG2IPlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<UG2ICameraDefaultsParameters> CameraDefaultsParameters;
	
	UPROPERTY()
	TArray<TObjectPtr<UCameraComponent>> CurrentCameraComponents;

	int32 CurrentCameraIndex = 0;

	EG2ICameraTypeEnum CurrentCameraType = EG2ICameraTypeEnum::FixedCamera;

	double OldCameraYawRotation = 0.;
	
	float DelayMovementTime = 0.f;
	
	bool bIsInitialized = false;

protected:
	
	UG2ICameraControllerComponent();

public:
	
	virtual void BeginPlay() override;
	
	// Interface methods
	UFUNCTION(BlueprintCallable, Category="Setup")
	virtual void SetupCurrentCamera_Implementation() override;
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void SwitchCameraBehavior_Implementation() override;

	UFUNCTION(BlueprintCallable, Category="Camera")
	virtual UCameraComponent* GetCameraComponent_Implementation() const override;

private:

	UFUNCTION()
	void AddCamera(UCameraComponent *AddedCamera);

	UFUNCTION()
	void RemoveCamera(UCameraComponent *RemovedCamera);

	UFUNCTION()
	void BroadcastCameraTypeAfterBlendFinish();

	void BroadcastCameraTypeAtBlendStart(const UCameraComponent& NewCamera);

	bool IsOwnerControllable() const;

	bool SetCamera(const UCameraComponent& NewCamera);

	bool SetCurrentCamera(int32 NewCameraIndex);
	
	void SetupDefaults();
	
	void BindDelegates();
	void BindPlayerControllerDelegates();

	void SetupCamerasDefaults();

	void SetupThirdPersonCameras() const;

	void SetupFixedCameras() const;

	void SetThirdPersonCameraYawRotation();

	void SetCurrentCameraIndex(int32 NewCameraIndex);
	
	void SetDefaultDelayMovementTime();
	
};
