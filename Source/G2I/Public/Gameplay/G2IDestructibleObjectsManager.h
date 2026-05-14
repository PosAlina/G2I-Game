#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "G2IDestructibleObjectsManager.generated.h"

class AG2IDestructibleActorBase;
class UG2ISoundComponent;

UCLASS()
class G2I_API AG2IDestructibleObjectsManager : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	AG2IDestructibleObjectsManager();
	int32 DestroySoundId = -1;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<TObjectPtr<AG2IDestructibleActorBase>> DestructibleObjectPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> ActorToDestroy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TObjectPtr<UG2ISoundComponent> SoundComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FName DestroySoundName = FName("DestroySound");
	
	void DestroyedObjectCounter();

private:
	int32 DestructibleObjNumber;
	int32 DestroyedObjCurrentNumber;
};
