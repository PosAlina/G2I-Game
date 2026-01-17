#pragma once

#include "CoreMinimal.h"
#include "G2IDestructibleActorInterface.h"
#include "GameFramework/Actor.h"
#include "G2IDestructibleActorBase.generated.h"

UCLASS()
class G2I_API AG2IDestructibleActorBase : public AActor, public IG2IDestructibleActorInterface
{
	GENERATED_BODY()
	
public:	
	AG2IDestructibleActorBase();
	virtual void DestroySelf_Implementation() override;

};
