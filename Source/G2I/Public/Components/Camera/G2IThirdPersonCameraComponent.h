#pragma once

#include "CoreMinimal.h"
#include "Camera/G2ICameraInterface.h"
#include "Camera/G2IThirdPersonCameraInputInterface.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "G2IThirdPersonCameraComponent.generated.h"

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
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> ThirdPersonCameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> ThirdPersonFollowCamera;
	
public:

	UG2IThirdPersonCameraComponent();

	virtual void OnRegister() override;

	virtual void BeginPlay() override;

	virtual void SetActive(bool bNewActive, bool bReset = false) override;

	// Interface methods
	UFUNCTION(BlueprintCallable, Category="Camera")
	virtual UCameraComponent* GetCameraComponent_Implementation() const override;
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void LookAction_Implementation(const float Yaw) override;

private:

	void SetupPassingThroughObjectCamera(USpringArmComponent& Camera);
	
};
