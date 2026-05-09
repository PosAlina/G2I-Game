#pragma once

#include "CoreMinimal.h"
#include "LaunchingIndication/G2ILauncherComponent.h"
#include "G2IActivationLauncherComponent.generated.h"

/**
 * LauncherComponent that also activates other actors
 * and can be launched only if CallActivation() was called.
 */
UCLASS(ClassGroup = (LaunchIndication), meta = (BlueprintSpawnableComponent))
class G2I_API UG2IActivationLauncherComponent : public UG2ILauncherComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<AActor>> ActorsToActivate;

private:
	bool bActivated;
	
public:
	UFUNCTION(BlueprintCallable)
	void CallActivation();

	virtual void SetIsLaunched(bool bNewIsLaunched) override;
};
