#include "Components/G2ICharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UG2ICharacterMovementComponent::OnRegister()
{
	Super::OnRegister();
	
	if (const ACharacter *Owner = Cast<ACharacter>(GetOwner()))
	{
		// Configure character movement
		Owner->GetCharacterMovement()->bOrientRotationToMovement = true;
		Owner->GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

		// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
		// instead of recompiling to adjust them
		Owner->GetCharacterMovement()->JumpZVelocity = 500.f;
		Owner->GetCharacterMovement()->AirControl = 0.35f;
		Owner->GetCharacterMovement()->MaxWalkSpeed = 500.f;
		Owner->GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
		Owner->GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
		Owner->GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
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

void UG2ICharacterMovementComponent::SetCanPassThroughObject(bool Value)
{
	bCanPassThroughObject = Value;
}

void UG2ICharacterMovementComponent::DisableJump_Implementation()
{
	if (ACharacter* Owner = Cast<ACharacter>(GetOwner()))
	{
		Owner->GetCharacterMovement()->GetNavAgentPropertiesRef().bCanJump = false;
	}
}

void UG2ICharacterMovementComponent::DisableMove_Implementation()
{
	if (ACharacter* Owner = Cast<ACharacter>(GetOwner()))
	{
		Owner->GetCharacterMovement()->GetNavAgentPropertiesRef().bCanWalk = false;
	}
}

void UG2ICharacterMovementComponent::DisableCrouch_Implementation()
{
	if (ACharacter* Owner = Cast<ACharacter>(GetOwner()))
	{
		Owner->GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = false;
	}
}

void UG2ICharacterMovementComponent::DisableRotation_Implementation() {
	if (ACharacter* Owner = Cast<ACharacter>(GetOwner())) {
		Owner->GetCharacterMovement()->bOrientRotationToMovement = false;
	}
}

void UG2ICharacterMovementComponent::EnableJump_Implementation()
{
	if (ACharacter* Owner = Cast<ACharacter>(GetOwner()))
	{
		Owner->GetCharacterMovement()->GetNavAgentPropertiesRef().bCanJump = true;
	}
}

void UG2ICharacterMovementComponent::EnableMove_Implementation()
{
	if (ACharacter* Owner = Cast<ACharacter>(GetOwner()))
	{
		Owner->GetCharacterMovement()->GetNavAgentPropertiesRef().bCanWalk = true;
	}
}

void UG2ICharacterMovementComponent::EnableCrouch_Implementation()
{
	if (ACharacter* Owner = Cast<ACharacter>(GetOwner()))
	{
		Owner->GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	}
}

void UG2ICharacterMovementComponent::EnableRotation_Implementation() {
	if (ACharacter* Owner = Cast<ACharacter>(GetOwner())) {
		Owner->GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}
