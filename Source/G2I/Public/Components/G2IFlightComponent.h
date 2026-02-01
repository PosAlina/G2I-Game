

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
	float FlightSpeed = 100.0f;
	
	bool bIsOnMaxHeight = false;
	int SpeedCoef = 100;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	virtual void Fly_Implementation(UActorComponent* MovementComponent, int Direction) override;
	virtual void StopFly_Implementation(UActorComponent* MovementComponent) override;
		
};
