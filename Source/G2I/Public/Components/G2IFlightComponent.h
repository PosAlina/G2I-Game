#pragma once

#include "CoreMinimal.h"
#include "G2IFlightInterface.h"
#include "Components/ActorComponent.h"
#include "G2IFlightComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2IFlightComponent : public UActorComponent, public IG2IFlightInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float FlightHeight = 500.0f;

	UPROPERTY(EditAnywhere)
	float FlightVelocity = 100.0f;

	UPROPERTY(EditAnywhere)
	float FlightMaxVelocity = 200.0f;
	
	bool bIsOnMaxHeight = false;
	int32 VelocityCoef = 100;

private:

	UPROPERTY()
	TObjectPtr<ACharacter> Owner;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

public:
	
	virtual void BeginPlay() override;

	// TODO: MovementComponent should be independent, so these functions should be transferred to it
	virtual bool Fly_Implementation(int Direction) override;
	virtual void StopFly_Implementation() override;

private:

	void SetupDefaults();
};
