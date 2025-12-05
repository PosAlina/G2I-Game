#include "Components/G2ICharacterMovementComponent.h"
#include "G2I.h"
#include "G2IPlayerState.h"
#include "GameFramework/Character.h"
#include "Components/G2IInteractionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "G2I.h"

UG2ICharacterMovementComponent::UG2ICharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UG2ICharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	BindingToDelegates();
}


void UG2ICharacterMovementComponent::OnRegister()
{
	Super::OnRegister();

	PreInitializationDefaults();
}

void UG2ICharacterMovementComponent::PreInitializationDefaults()
{
	AActor *OwnerActor = GetOwner();
	if (!ensure(OwnerActor))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
		return;
	}
	
	ACharacter *Owner = Cast<ACharacter>(OwnerActor);
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}
	
	Owner->bUseControllerRotationPitch = false;
	Owner->bUseControllerRotationYaw = false;
	Owner->bUseControllerRotationRoll = false;
		
	UCharacterMovementComponent *CurrentCharacterMovement = Owner->GetCharacterMovement();
	if (!ensure(CurrentCharacterMovement))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner %s hasn't character movement component in %s"),
			*Owner->GetName(), *GetName());
		return;
	}
	
	CurrentCharacterMovement->bOrientRotationToMovement = true;
	CurrentCharacterMovement->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	
	CurrentCharacterMovement->JumpZVelocity = 500.f;
	CurrentCharacterMovement->AirControl = 0.35f;
	CurrentCharacterMovement->MaxWalkSpeed = 500.f;
	StandartMaxWalkSpeed = CurrentCharacterMovement->MaxWalkSpeed;
	CurrentCharacterMovement->MinAnalogWalkSpeed = 20.f;
	CurrentCharacterMovement->BrakingDecelerationWalking = 2000.f;
	CurrentCharacterMovement->BrakingDecelerationFalling = 1500.0f;
	
	CurrentCharacterMovement->GetNavAgentPropertiesRef().bCanCrouch = true;
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
					bIsCrouchState = false;
				}
				else
				{
					Owner->Crouch();
					bIsCrouchState = true;
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

void UG2ICharacterMovementComponent::ToggleJump()
{
	if (ACharacter* Owner = Cast<ACharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent* CurrentCharacterMovement = Owner->GetCharacterMovement())
		{
			CurrentCharacterMovement->GetNavAgentPropertiesRef().bCanJump = !CurrentCharacterMovement->GetNavAgentPropertiesRef().bCanJump;
		}
	}
}

void UG2ICharacterMovementComponent::ToggleMove()
{
	if (ACharacter* Owner = Cast<ACharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent* CurrentCharacterMovement = Owner->GetCharacterMovement())
		{
			CurrentCharacterMovement->GetNavAgentPropertiesRef().bCanWalk = !CurrentCharacterMovement->GetNavAgentPropertiesRef().bCanWalk;
		}
	}
}

void UG2ICharacterMovementComponent::ToggleCrouch()
{
	if (ACharacter* Owner = Cast<ACharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent* CurrentCharacterMovement = Owner->GetCharacterMovement())
		{
			CurrentCharacterMovement->GetNavAgentPropertiesRef().bCanCrouch = !CurrentCharacterMovement->GetNavAgentPropertiesRef().bCanCrouch;
		}
	}
}

void UG2ICharacterMovementComponent::ToggleRotation() {
	if (ACharacter* Owner = Cast<ACharacter>(GetOwner())) {
		if (UCharacterMovementComponent* CurrentCharacterMovement = Owner->GetCharacterMovement())
		{
			CurrentCharacterMovement->bOrientRotationToMovement = !CurrentCharacterMovement->bOrientRotationToMovement;
		}
	}
}

void UG2ICharacterMovementComponent::ToggleSlow(float SpeedChange)
{
	if (SpeedChange < 0) {
		UE_LOG(LogG2I, Log, TEXT("Speed can't be negative"));
		return;
	}

	if (ACharacter* Owner = Cast<ACharacter>(GetOwner())) {
		if (UCharacterMovementComponent* CurrentCharacterMovement = Owner->GetCharacterMovement())
		{
			float CurrentMaxWalkSpeed = CurrentCharacterMovement->MaxWalkSpeed;
			if (FMath::IsNearlyEqual(CurrentMaxWalkSpeed, StandartMaxWalkSpeed)) {
				CurrentCharacterMovement->MaxWalkSpeed -= SpeedChange < CurrentMaxWalkSpeed ? SpeedChange : CurrentMaxWalkSpeed;
			}
			else {
				CurrentCharacterMovement->MaxWalkSpeed = StandartMaxWalkSpeed;
			}
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
void UG2ICharacterMovementComponent::BindingToDelegates()
{
	if (const UWorld *World = GetWorld())
	{
		if (const APlayerController *PlayerController = World->GetFirstPlayerController())
		{
			if (AG2IPlayerState *PlayerState = PlayerController->GetPlayerState<AG2IPlayerState>())
			{
				PlayerState->OnNewControllerPossessDelegate.AddDynamic(this, &ThisClass::PossessedByNewController);
			}
			else
			{
				UE_LOG(LogG2I, Error, TEXT("G2I PlayerState doesn't exist"));
			}
		}
		else
		{
			UE_LOG(LogG2I, Error, TEXT("PlayerController doesn't exist"));
		}
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist"));
	}
	
	if (const ACharacter* Owner = Cast<ACharacter>(GetOwner()))
	{
		if (UG2IInteractionComponent* InteractionComp = Owner->FindComponentByClass<UG2IInteractionComponent>()) {
			InteractionComp->OnMovingInteractingDelegate.AddDynamic(this, &UG2ICharacterMovementComponent::HandleMovingInteraction);
		}
		else{
			UE_LOG(LogG2I, Error, TEXT("Character dont't have Interaction Component"));
		}
	}
	else{
		UE_LOG(LogG2I, Error, TEXT("Component Owner isn't Character"));
	}
}


void UG2ICharacterMovementComponent::PossessedByNewController(APawn *ChangedPawn)
{
	if (ACharacter *Owner = Cast<ACharacter>(GetOwner()))
	{
		if (ChangedPawn == Owner)
		{
			if (bIsCrouchState)
			{
				Owner->Crouch();
			}
		}
	}
}
