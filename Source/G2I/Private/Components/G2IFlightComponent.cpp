


#include "Components/G2IFlightComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UG2IFlightComponent::Fly_Implementation(UActorComponent* MovementComponent, int Direction)
{
	AActor* Owner = GetOwner();
	if (Owner)
	{
		FVector ActorLocation = Owner->GetActorLocation();
		UCharacterMovementComponent* CharMovementComp = StaticCast<UCharacterMovementComponent*>(MovementComponent);
		if (CharMovementComp)
		{
			int CurrentVelocity = CharMovementComp->Velocity.Z;
			CharMovementComp->GravityScale = 0.0f;
			
			if (Direction > 0 && ActorLocation.Z < FlightHeight && !bIsOnMaxHeight)
			{
				if (FMath::Abs(CurrentVelocity) < FlightMaxVelocity)
				{
					CharMovementComp->AddForce(FVector(0, 0, Direction * FlightVelocity * VelocityCoef));
				}
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
				if (FMath::Abs(CurrentVelocity) < FlightMaxVelocity)
				{
					CharMovementComp->AddForce(FVector(0, 0, Direction * FlightVelocity * VelocityCoef));
				}
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
	if (CharMovementComp->IsMovingOnGround())
	{
		CharMovementComp->GravityScale = 1.0f;
	}
}

