


#include "Components/G2IFlightComponent.h"

#include "AITypes.h"
#include "GameFramework/CharacterMovementComponent.h"

void UG2IFlightComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UG2IFlightComponent::Fly_Implementation(UActorComponent* MovementComponent, int Direction)
{
	AActor* Owner = GetOwner();
	if (Owner)
	{
		FVector ActorLocation = Owner->GetActorLocation();
		UCharacterMovementComponent* CharMovementComp = StaticCast<UCharacterMovementComponent*>(MovementComponent);
		if (CharMovementComp)
		{
			if (Direction > 0 && ActorLocation.Z < FlightHeight && !bIsOnMaxHeight)
			{
				CharMovementComp->AddForce(FVector(0, 0, FlightSpeed * SpeedCoef));
			}
			else if (Direction > 0 && !bIsOnMaxHeight)
			{
				CharMovementComp->ClearAccumulatedForces();
				CharMovementComp->Velocity.Z = 0.0f;
				bIsOnMaxHeight = true;
			}
			if (Direction < 0)
			{
				bIsOnMaxHeight = false;
				CharMovementComp->AddForce(FVector(0, 0, -FlightSpeed * SpeedCoef));
			}
		}
	}
}

void UG2IFlightComponent::StopFly_Implementation(UActorComponent* MovementComponent)
{
	UCharacterMovementComponent* CharMovementComp = StaticCast<UCharacterMovementComponent*>(MovementComponent);
	if (CharMovementComp)
	{
		CharMovementComp->ClearAccumulatedForces();
		CharMovementComp->Velocity.Z = 0.0f;
	}
}

