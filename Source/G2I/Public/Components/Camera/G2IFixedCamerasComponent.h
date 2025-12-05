#pragma once

#include "CoreMinimal.h"
#include "Camera/G2ICameraInterface.h"
#include "Components/ActorComponent.h"
#include "G2IFixedCamerasComponent.generated.h"


class USpringArmComponent;

UCLASS(ClassGroup=(Camera), meta=(BlueprintSpawnableComponent))
class G2I_API UG2IFixedCamerasComponent : public UActorComponent, public IG2ICameraInterface
{
	GENERATED_BODY()

public:

	// Interface methods
	UFUNCTION(BlueprintCallable, Category="Camera")
	virtual UCameraComponent* GetCameraComponent_Implementation() const override;

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
	
public:

	UG2IFixedCamerasComponent();

	virtual void BeginPlay() override;

	virtual void OnRegister() override;

private:

	void BindingToDelegates();
	
};
