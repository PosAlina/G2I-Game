#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "G2IDestructibleObjectsManager.generated.h"

class AG2IDestructibleActorBase;

UCLASS()
class G2I_API AG2IDestructibleObjectsManager : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<TObjectPtr<AG2IDestructibleActorBase>> DestructibleObjectPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> ActorToDestroy;
	
	void DestroyedObjectCounter();

private:
	int32 DestructibleObjNumber;
	int32 DestroyedObjCurrentNumber;
};
