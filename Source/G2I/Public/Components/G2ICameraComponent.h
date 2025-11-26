#pragma once

#include "CoreMinimal.h"
#include "G2ICameraInputInterface.h"
#include "Components/ActorComponent.h"
#include "G2ICameraComponent.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class G2I_API UG2ICameraComponent : public USceneComponent, public IG2ICameraInputInterface
{
	GENERATED_BODY()

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
public:

	UG2ICameraComponent();

	virtual void OnRegister() override;

	// Interface methods
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void LookAction_Implementation(const float Yaw, const float Pitch) override;

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void MouseLookAction_Implementation(const float Yaw, const float Pitch) override;

private:

	void SetupPassingThroughObjectCamera(const ACharacter& Owner, USpringArmComponent& Camera);
	
};
