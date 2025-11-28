

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
	// Sets default values for this actor's properties
	AG2IDestructibleActorBase();
	virtual void DestroySelf_Implementation() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
