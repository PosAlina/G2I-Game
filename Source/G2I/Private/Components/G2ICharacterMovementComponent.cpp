#include "Components/G2ICharacterMovementComponent.h"
#include "G2I.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UG2ICharacterMovementComponent::OnRegister()
{
	Super::OnRegister();
	
	if (const ACharacter *Owner = Cast<ACharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent *CurrentCharacterMovement = Owner->GetCharacterMovement())
		{
			CurrentCharacterMovement->bOrientRotationToMovement = true;
			CurrentCharacterMovement->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
			
			CurrentCharacterMovement->JumpZVelocity = 500.f;
			CurrentCharacterMovement->AirControl = 0.35f;
			CurrentCharacterMovement->MaxWalkSpeed = 500.f;
			CurrentCharacterMovement->MinAnalogWalkSpeed = 20.f;
			CurrentCharacterMovement->BrakingDecelerationWalking = 2000.f;
			CurrentCharacterMovement->BrakingDecelerationFalling = 1500.0f;
			
			CurrentCharacterMovement->GetNavAgentPropertiesRef().bCanCrouch = true;
		}
	}
}

void UG2ICharacterMovementComponent::MoveAction_Implementation(const float Right, const float Forward, const FRotator Rotation)
{
	if (ACharacter *Owner = Cast<ACharacter>(GetOwner()))
	{
		// find out which way is forward
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		Owner->AddMovementInput(ForwardDirection, Forward);
		Owner->AddMovementInput(RightDirection, Right);
	}
}
	
void UG2ICharacterMovementComponent::JumpAction_Implementation()
{
	if (ACharacter *Owner = Cast<ACharacter>(GetOwner()))
	{
		Owner->Jump();
	}
}

void UG2ICharacterMovementComponent::StopJumpingAction_Implementation()
{
	if (ACharacter *Owner = Cast<ACharacter>(GetOwner()))
	{
		Owner->StopJumping();
	}
}

void UG2ICharacterMovementComponent::ToggleCrouchAction_Implementation()
{
	if (ACharacter *Owner = Cast<ACharacter>(GetOwner()))
	{
		if (const UCharacterMovementComponent *CurrentCharacterMovement = Owner->GetCharacterMovement())
		{
			if (!CurrentCharacterMovement->IsFalling())
			{
				if (Owner->IsCrouched())
				{
					Owner->UnCrouch();
				}
				else
				{
					Owner->Crouch();
				}
			}
		}
		else
		{
			UE_LOG(LogG2I, Warning, TEXT("%s doesn't have base character movement component"),
				*Owner->GetName());
		}
	}
}

void UG2ICharacterMovementComponent::SetCanPassThroughObject(bool Value)
{
	bCanPassThroughObject = Value;
}
