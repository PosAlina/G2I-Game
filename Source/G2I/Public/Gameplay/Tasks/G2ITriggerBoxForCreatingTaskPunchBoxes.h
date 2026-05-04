#pragma once

#include "CoreMinimal.h"
#include "Tasks/G2ITriggerBoxForCreatingTask.h"
#include "G2ITriggerBoxForCreatingTaskPunchBoxes.generated.h"

class AG2IDestructibleActorBase;

UENUM(BlueprintType)
enum class EG2ICountShouldDestructActors : uint8
{
	OneActor UMETA(DisplayName = "One actor"),
	AllActors UMETA(DisplayName = "All actors")
};

UCLASS()
class G2I_API AG2ITriggerBoxForCreatingTaskPunchBoxes : public AG2ITriggerBoxForCreatingTask
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, Category = Conditions)
	TSet<TObjectPtr<AG2IDestructibleActorBase>> Boxes;
	
	UPROPERTY(EditAnywhere, Category = Conditions)
	EG2ICountShouldDestructActors CountShouldDestructActors = EG2ICountShouldDestructActors::AllActors;

public:
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
protected:
	
	virtual void CheckAndCreateTask(AActor *OtherActor) override;
	
};
