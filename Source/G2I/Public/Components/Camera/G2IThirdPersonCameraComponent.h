#pragma once

#include "CoreMinimal.h"
#include "Camera/G2ICameraInterface.h"
#include "Camera/G2IThirdPersonCameraInputInterface.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "G2IThirdPersonCameraComponent.generated.h"

class UG2ICameraDefaultsParameters;
class ACharacter;
class UCameraComponent;
class USpringArmComponent;

UCLASS(ClassGroup=(Camera), meta=(BlueprintSpawnableComponent))
class G2I_API UG2IThirdPersonCameraComponent : public USceneComponent, public IG2ICameraInterface,
	public IG2IThirdPersonCameraInputInterface
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TObjectPtr<ACharacter> Owner;
	
	UPROPERTY()
	TObjectPtr<UActorComponent> CameraController;

	UPROPERTY(BlueprintAssignable)
	FAddCameraDelegate OnAddCameraDelegate;

	UPROPERTY(BlueprintAssignable)
	FRemoveCameraDelegate OnRemoveCameraDelegate;

	UPROPERTY()
	TObjectPtr<UG2ICameraDefaultsParameters> CameraDefaultsParameters;
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> ThirdPersonCameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> ThirdPersonFollowCamera;
	
protected:

	UG2IThirdPersonCameraComponent();

	virtual void OnRegister() override;

	virtual void BeginPlay() override;

public:

	// Interface methods
	UFUNCTION(BlueprintCallable, Category="Camera")
	virtual TArray<UCameraComponent *> GetCameraComponents_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category="Camera")
	virtual void SetupCameras_Implementation() override;

	UFUNCTION()
	virtual FAddCameraDelegate& GetAddCameraDelegate() override;

	UFUNCTION()
	virtual FRemoveCameraDelegate& GetRemoveCameraDelegate() override;
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void LookAction_Implementation(const float Yaw, const float Pitch) override;

private:
	
	void SetupDefaults();

	void PreInitializeDefaults();

	void InitializeCameraBoom();

	void InitializeFollowCamera();
	
};
