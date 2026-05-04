#pragma once

#include "CoreMinimal.h"
#include "Tasks/G2ITriggerBoxForCreatingTask.h"
#include "G2ITriggerBoxForCreatingTaskShootOnGear.generated.h"

class AG2IRotatingBySteamGear;

UCLASS()
class G2I_API AG2ITriggerBoxForCreatingTaskShootOnGear : public AG2ITriggerBoxForCreatingTask
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, Category = Conditions)
	TObjectPtr<AG2IRotatingBySteamGear> Gear;

public:
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
protected:
	
	virtual void CheckAndCreateTask(AActor *OtherActor) override;
	
};
