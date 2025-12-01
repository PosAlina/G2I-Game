#include "Components/G2ICharacterCollisionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/G2ICharacterMovementComponent.h"
#include "GameFramework/Character.h"

void UG2ICharacterCollisionComponent::OnRegister()
{
	Super::OnRegister();
	
	if (const ACharacter *Owner = Cast<ACharacter>(GetOwner()))
	{
		if (UCapsuleComponent *CapsuleComp = Owner->GetCapsuleComponent())
		{
			CapsuleComp->InitCapsuleSize(42.f, 96.0f);

			SetupPassingThroughObjectCollision(*Owner, *CapsuleComp);
		}
	}
}

void UG2ICharacterCollisionComponent::SetupPassingThroughObjectCollision(const ACharacter& Owner,
	UCapsuleComponent& CapsuleComp)
{
	if (UActorComponent *Component = Owner.GetComponentByClass(UG2ICharacterMovementComponent::StaticClass()))
	{
		if (const UG2ICharacterMovementComponent *MovementComponent = Cast<UG2ICharacterMovementComponent>(Component))
		{
			if (MovementComponent->CanPassThroughObject())
			{
				CapsuleComp.SetCollisionObjectType(PassingThroughCharacterCollision);
			}
		}
	}
}
