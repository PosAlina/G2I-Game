#pragma once

#include "CoreMinimal.h"
#include "G2ITriggerBoxForCreatingTask.h"
#include "AG2ITriggerBoxForCreatingTaskFollowAI.generated.h"

UCLASS()
class G2I_API AAG2ITriggerBoxForCreatingTaskFollowAI : public AG2ITriggerBoxForCreatingTask
{
	GENERATED_BODY()

public:
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
protected:
	
	virtual void CheckAndCreateTask(AActor *OtherActor) override;

};
