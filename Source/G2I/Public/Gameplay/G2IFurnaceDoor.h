#pragma once

#include "CoreMinimal.h"
#include "Gameplay/G2IMovingByGearWithSplineActor.h"
#include "Interfaces/G2IActivationInterface.h"
#include "G2IFurnaceDoor.generated.h"


class UG2ISoundComponent;

UCLASS()
class G2I_API AG2IFurnaceDoor : public AG2IMovingByGearWithSplineActor, public IG2IActivationInterface
{
	GENERATED_BODY()
private:
	int32 FurnaceDoorOpenSoundId = -1;

protected:
	FTimerHandle PushTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Data|Activation")
	float TotalTime = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Data|Activation")
	int32 MaxTicks = 10;

	int32 CurrentTicks = 0;

	void PerformTick();
public:	
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
		TObjectPtr<UG2ISoundComponent> SoundComp;

	AG2IFurnaceDoor();
	virtual void BeginPlay();

	UPROPERTY(EditAnywhere, Category = "Data|Spline")
	float Force = 0.1f;

	UFUNCTION(BlueprintCallable)
	void StartTick();

	virtual void Activate_Implementation() override;

	virtual void Deactivate_Implementation() override;
};
