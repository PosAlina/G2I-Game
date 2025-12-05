#pragma once

#include "CoreMinimal.h"
#include "Camera/G2ICameraControllerInputInterface.h"
#include "Camera/G2ICameraInterface.h"
#include "Components/ActorComponent.h"
#include "G2ICameraControllerComponent.generated.h"


class UCameraComponent;
class UG2IFixedCamerasComponent;
class UG2IThirdPersonCameraComponent;

UCLASS(ClassGroup=(Camera), meta=(BlueprintSpawnableComponent))
class G2I_API UG2ICameraControllerComponent : public UActorComponent, public IG2ICameraControllerInputInterface
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY()
	TArray<TObjectPtr<UActorComponent>> CameraComponents;

	uint32 CurrentCameraIndex = 0;

	UPROPERTY()
	TObjectPtr<UCameraComponent> CurrentCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, Units="s"))
	float BlendTime = 0.5f;

public:

	virtual void BeginPlay() override;

	// Interface methods
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void SwitchCameraBehavior_Implementation() override;

	UFUNCTION(BlueprintCallable, Category="Setup")
	virtual void SetupCameraComponents_Implementation() override;

	UFUNCTION(BlueprintCallable, Category="Camera")
	virtual UCameraComponent* GetCameraComponent_Implementation() const override;

private:

	void SetupCameraControllersDefaults();

	void SetupPlayerController();

	bool SetCurrentCamera(const int32 CameraIndex);

	void SetNextCameraWithStartIndex(const int32 CameraIndex);
	
};
