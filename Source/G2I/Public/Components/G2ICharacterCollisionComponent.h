#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "G2ICharacterCollisionComponent.generated.h"


class UCapsuleComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class G2I_API UG2ICharacterCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	ECollisionChannel PassingThroughCharacterCollision = ECC_GameTraceChannel2;
	
public:

	virtual void PostInitProperties() override;

private:

	void SetupPassingThroughObjectCollision(const ACharacter& Owner, UCapsuleComponent& CapsuleComp);
	
};
