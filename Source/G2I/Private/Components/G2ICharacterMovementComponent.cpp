#include "Components/G2ICharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/G2IInteractionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "G2I.h"

void UG2ICharacterMovementComponent::BindingToDelegates()
{
	if (const ACharacter* Owner = Cast<ACharacter>(GetOwner()))
	{
		if (UG2IInteractionComponent* InteractionComp = Owner->FindComponentByClass<UG2IInteractionComponent>()) {
			InteractionComp->OnMovingInteractingDelegate.AddDynamic(this, &UG2ICharacterMovementComponent::HandleMovingInteraction);
		}
	}
}

void UG2ICharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	BindingToDelegates();
}

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
		StandartMaxWalkSpeed = Owner->GetCharacterMovement()->MaxWalkSpeed;
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
		OnJumpDelegate.Broadcast();
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

void UG2ICharacterMovementComponent::ToggleJump()
{
	if (ACharacter* Owner = Cast<ACharacter>(GetOwner()))
	{
		Owner->GetCharacterMovement()->GetNavAgentPropertiesRef().bCanJump = 1 - Owner->GetCharacterMovement()->GetNavAgentPropertiesRef().bCanJump;
	}
}

void UG2ICharacterMovementComponent::ToggleMove()
{
	if (ACharacter* Owner = Cast<ACharacter>(GetOwner()))
	{
		Owner->GetCharacterMovement()->GetNavAgentPropertiesRef().bCanWalk = 1 - Owner->GetCharacterMovement()->GetNavAgentPropertiesRef().bCanWalk;
	}
}

void UG2ICharacterMovementComponent::ToggleCrouch()
{
	if (ACharacter* Owner = Cast<ACharacter>(GetOwner()))
	{
		Owner->GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = 1 - Owner->GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch;
	}
}

void UG2ICharacterMovementComponent::ToggleRotation() {
	if (ACharacter* Owner = Cast<ACharacter>(GetOwner())) {
		Owner->GetCharacterMovement()->bOrientRotationToMovement = 1 - Owner->GetCharacterMovement()->bOrientRotationToMovement;
	}
}

void UG2ICharacterMovementComponent::ToggleSlow(float SpeedChange)
{
	if (SpeedChange < 0) {
		UE_LOG(LogG2I, Log, TEXT("Speed can't be negative"));
		return;
	}

	if (ACharacter* Owner = Cast<ACharacter>(GetOwner())) {
		float currentMaxWalkSpeed = Owner->GetCharacterMovement()->MaxWalkSpeed;
		if (abs(currentMaxWalkSpeed - StandartMaxWalkSpeed) <= 1e-5) {
			Owner->GetCharacterMovement()->MaxWalkSpeed -= SpeedChange < currentMaxWalkSpeed ? SpeedChange : currentMaxWalkSpeed;
		}
		else {
			Owner->GetCharacterMovement()->MaxWalkSpeed = StandartMaxWalkSpeed;
		}
	}
}

void UG2ICharacterMovementComponent::HandleMovingInteraction(float SpeedChange)
{
	ToggleCrouch();
	ToggleJump();
	ToggleRotation();
	ToggleSlow(SpeedChange);
}