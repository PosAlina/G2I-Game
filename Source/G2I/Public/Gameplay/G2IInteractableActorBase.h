#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "G2IInteractableActorBase.generated.h"

UCLASS()
class G2I_API AG2IInteractableActorBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AG2IInteractableActorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
