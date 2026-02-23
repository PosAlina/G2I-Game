#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "G2IOverlapActorsActivator.generated.h"

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

private:

	UPROPERTY()
	TObjectPtr<UWorld> World;
	
public:

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

protected:

	virtual void BeginPlay() override;

};
