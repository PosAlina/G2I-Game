#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "G2ICharacterCollisionComponent.generated.h"

class ACharacter;
class UCapsuleComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterCollisionOverlap);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class G2I_API UG2ICharacterCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	ECollisionChannel PassingThroughCharacterCollision = ECC_GameTraceChannel2;

public:

	UPROPERTY(BlueprintAssignable)
	FCharacterCollisionOverlap OnCharacterCollisionOverlap;
	
public:

	virtual void OnRegister() override;

private:

	UFUNCTION()
	void OnCapsuleBeginOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
		UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SetupPassingThroughObjectCollision(const ACharacter& Owner, UCapsuleComponent& CapsuleComp);
	
};
