#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "G2IOverlapActorsActivator.generated.h"

class UG2IActivationLauncherComponent;

/**
 * In this zone, actors with the required tag are activated and deactivated when exiting.
 */
UCLASS()
class G2I_API AG2IOverlapActorsActivator : public ATriggerBox
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlapping activator")
	FName CheckerTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlapping activator")
	bool bEnableActivateBeginOverlappingActors = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlapping activator")
	bool bEnableDeactivateEndOverlappingActors = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlapping activator")
	TObjectPtr<USoundBase> ActivateSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlapping activator")
	TObjectPtr<USoundBase> DeactivateSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UG2IActivationLauncherComponent> LauncherComp;

private:

	UPROPERTY()
	TObjectPtr<UWorld> World;
	
public:
	AG2IOverlapActorsActivator();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

protected:

	virtual void BeginPlay() override;

};
