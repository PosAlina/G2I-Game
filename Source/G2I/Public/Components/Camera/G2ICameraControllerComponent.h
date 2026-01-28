#pragma once

#include "CoreMinimal.h"
#include "G2ICameraTypeEnum.h"
#include "G2ICameraControllerInputInterface.h"
#include "Components/ActorComponent.h"
#include "G2ICameraControllerComponent.generated.h"

class UG2ICameraDefaultsParameters;
class AG2IPlayerController;
class UCameraComponent;
class UG2IFixedCamerasComponent;
class UG2IThirdPersonCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetCameraTypeDelegate, EG2ICameraTypeEnum, CurrentCameraType);

UCLASS(ClassGroup=(Camera), meta=(BlueprintSpawnableComponent))
class G2I_API UG2ICameraControllerComponent : public UActorComponent, public IG2ICameraControllerInputInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FSetCameraTypeDelegate OnSetCameraTypeDelegate;
	
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

protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

public:
	
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

	bool IsOwnerControllable() const;

	bool SetCamera(const UCameraComponent& NewCamera);

	bool SetCurrentCamera(int32 NewCameraIndex);
	
	void SetupDefaults();
	
	void BindDelegates();

	void SetupCamerasDefaults();

	void SetupThirdPersonCameras() const;

	void SetupFixedCameras() const;
	
};
