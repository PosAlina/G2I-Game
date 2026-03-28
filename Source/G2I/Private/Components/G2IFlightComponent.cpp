#include "Components/G2IFlightComponent.h"
#include "G2I.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UG2IFlightComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupDefaults();
}

void UG2IFlightComponent::SetupDefaults()
{
	Owner = Cast<ACharacter>(GetOwner());
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find character owner"), *GetName());
		return;
	}
	MovementComponent = Owner->GetCharacterMovement();
	if (!ensure(MovementComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find movement component in %s"), *GetName(),
			*Owner->GetActorNameOrLabel());
		return;
	}
}

void UG2IFlightComponent::Fly_Implementation(const int Direction)
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find character owner"), *GetName());
		return;
	}
	if (!ensure(MovementComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find movement component in %s"), *GetName(),
			*Owner->GetActorNameOrLabel());
		return;
	}

	const FVector ActorLocation = Owner->GetActorLocation();
	const int32 CurrentVelocity = MovementComponent->Velocity.Z;
	MovementComponent->GravityScale = 0.0f;
	
	if (Direction > 0 && ActorLocation.Z < FlightHeight && !bIsOnMaxHeight)
	{
		if (FMath::Abs(CurrentVelocity) < FlightMaxVelocity)
		{
			MovementComponent->AddForce(FVector(0, 0, Direction * FlightVelocity * VelocityCoef));
		}
	}
	else if (Direction > 0 && !bIsOnMaxHeight)
	{
		MovementComponent->ClearAccumulatedForces();
		MovementComponent->Velocity.Z = 0.0f;
		bIsOnMaxHeight = true;
	}
	
	if (Direction < 0)
	{
		bIsOnMaxHeight = false;
		if (FMath::Abs(CurrentVelocity) < FlightMaxVelocity)
		{
			MovementComponent->AddForce(FVector(0, 0, Direction * FlightVelocity * VelocityCoef));
		}
	}
}

void UG2IFlightComponent::StopFly_Implementation()
{
	if (!ensure(MovementComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find movement component"), *GetName());
		return;
	}
	
	MovementComponent->ClearAccumulatedForces();
	MovementComponent->Velocity.Z = 0.0f;
	if (MovementComponent->IsMovingOnGround())
	{
		MovementComponent->GravityScale = 1.0f;
	}
}

