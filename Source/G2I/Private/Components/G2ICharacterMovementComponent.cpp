#include "G2ICharacterMovementComponent.h"
#include "G2I.h"
#include "G2IPlayerState.h"
#include "GameFramework/Character.h"
#include "G2IInteractionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "G2IAimingComponent.h"
#include "G2ICameraControllerComponent.h"
#include "G2ICameraDefaultsParameters.h"
#include "G2ICameraStateEnums.h"
#include "G2IPlayerController.h"
#include "Components/CapsuleComponent.h"

UG2ICharacterMovementComponent::UG2ICharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bOrientRotationToMovement = true;
	RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	
	JumpZVelocity = 500.f;
	AirControl = 0.35f;
	MaxWalkSpeed = 500.f;
	StandartMaxWalkSpeed = MaxWalkSpeed;
	MinAnalogWalkSpeed = 20.f;
	BrakingDecelerationWalking = 2000.f;
	BrakingDecelerationFalling = 1500.0f;
	MaxSimulationIterations = 10;
	MaxSimulationTimeStep = 0.06f;
}

void UG2ICharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetupDefaults();
	BindingToDelegates();
}

void UG2ICharacterMovementComponent::SetupDefaults()
{
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}
	PlayerController = Cast<AG2IPlayerController>(World->GetFirstPlayerController());
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
			*GetName(), *AG2IPlayerController::StaticClass()->GetName());
		return;
	}

	CameraDefaultsParameters = PlayerController->GetCameraDefaultsParameters();
	if (!ensure(CameraDefaultsParameters))
	{
		UE_LOG(LogG2I, Error, TEXT("%s can not return camera defaults parameters in %s in %s"),
			*PlayerController->GetName(), *GetName(), *Owner->GetActorNameOrLabel());
		return;
	}
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
	
	Owner = Cast<ACharacter>(OwnerActor);
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}
	
	DisableRotationTowardsCamera();

	GetNavAgentPropertiesRef().bCanCrouch = true;

	World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}
}

void UG2ICharacterMovementComponent::MoveAction_Implementation(const float Right, const float Forward, const FRotator Rotation)
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}
	
	// find out which way is forward
	const FRotator YawRotation(0, Rotation.Yaw + CameraPendingYawRotation, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement 
	Owner->AddMovementInput(ForwardDirection, Forward);
	Owner->AddMovementInput(RightDirection, Right);
}
	
void UG2ICharacterMovementComponent::JumpAction_Implementation()
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}
	
	Owner->Jump();
	OnJumpDelegate.Broadcast();
}

void UG2ICharacterMovementComponent::StopJumpingAction_Implementation()
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}
	
	Owner->StopJumping();
}

void UG2ICharacterMovementComponent::ImpulseJumpAction_Implementation(const float ImpulseStrength)
{
	const FVector JumpImpulse = FVector(0, 0, ImpulseStrength);
	AddImpulse(JumpImpulse, true);
}

bool UG2ICharacterMovementComponent::IsInAir_Implementation()
{
	return IsFalling();
}

void UG2ICharacterMovementComponent::ToggleCrouchAction_Implementation()
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}
	
	if (!IsFalling())
	{
		if (Owner->IsCrouched())
		{
			if (CanUncrouch())
			{
				Owner->UnCrouch();
				bIsCrouchState = false;
			}
		}
		else
		{
			Owner->Crouch();
			bIsCrouchState = true;
		}
	}
}

void UG2ICharacterMovementComponent::SetCanPassThroughObject(const bool Value)
{
	bCanPassThroughObject = Value;
}

void UG2ICharacterMovementComponent::ToggleJump()
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}

	GetNavAgentPropertiesRef().bCanJump = !GetNavAgentPropertiesRef().bCanJump;
}

void UG2ICharacterMovementComponent::ToggleMove()
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}
	
	GetNavAgentPropertiesRef().bCanWalk = !GetNavAgentPropertiesRef().bCanWalk;
}

void UG2ICharacterMovementComponent::ToggleCrouch()
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}
	
	GetNavAgentPropertiesRef().bCanCrouch = !GetNavAgentPropertiesRef().bCanCrouch;
}

void UG2ICharacterMovementComponent::ToggleRotation() {
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}

	bOrientRotationToMovement = !bOrientRotationToMovement;
}

void UG2ICharacterMovementComponent::ToggleSlow(const float SpeedChange)
{
	if (SpeedChange < 0) {
		UE_LOG(LogG2I, Log, TEXT("Speed can't be negative"));
		return;
	}

	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}
	
	float CurrentMaxWalkSpeed = MaxWalkSpeed;
	if (FMath::IsNearlyEqual(CurrentMaxWalkSpeed, StandartMaxWalkSpeed)) {
		MaxWalkSpeed -= SpeedChange < CurrentMaxWalkSpeed ? SpeedChange : CurrentMaxWalkSpeed;
	}
	else {
		MaxWalkSpeed = StandartMaxWalkSpeed;
	}
}

void UG2ICharacterMovementComponent::HandleMovingInteraction(const float SpeedChange)
{
	ToggleCrouch();
	ToggleJump();
	ToggleRotation();
	ToggleSlow(SpeedChange);
}
void UG2ICharacterMovementComponent::BindingToDelegates()
{
	BindingOwnerComponentsDelegates();
	
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("PlayerController doesn't exist in %s"), *GetName());
		return;
	}
	
	AG2IPlayerState *PlayerState = PlayerController->GetPlayerState<AG2IPlayerState>();
	if (!ensure(PlayerState))
	{
		UE_LOG(LogG2I, Error, TEXT("G2I PlayerState doesn't exist in %s"), *GetName());
		return;
	}
	
	PlayerState->OnNewControllerPossessDelegate.AddDynamic(this, &ThisClass::PossessedByNewController);
}

void UG2ICharacterMovementComponent::BindingOwnerComponentsDelegates()
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}
	
	if (UG2IInteractionComponent* InteractionComp = Owner->FindComponentByClass<UG2IInteractionComponent>()) {
		InteractionComp->OnMovingInteractingDelegate.AddDynamic(this, &UG2ICharacterMovementComponent::HandleMovingInteraction);
	}
	else{
		UE_LOG(LogG2I, Error, TEXT("Character dont't have Interaction Component"));
	}

	if (UG2IAimingComponent *AimingComponent = Owner->FindComponentByClass<UG2IAimingComponent>())
	{
		AimingComponent->OnStartAimingDelegate.AddDynamic(this, &ThisClass::EnableRotationTowardsCamera);
		AimingComponent->OnFinishAimingDelegate.AddDynamic(this, &ThisClass::DisableRotationTowardsCamera);
	}

	if (UG2ICameraControllerComponent *CameraControllerComponent = Owner->FindComponentByClass<UG2ICameraControllerComponent>())
	{
		CameraControllerComponent->OnSetThirdPersonCameraTypeDelegate.AddDynamic(this, &ThisClass::SetMovementWithThirdPersonCamera);
		CameraControllerComponent->OnSetFixedCameraTypeDelegate.AddDynamic(this, &ThisClass::SetMovementWithFixedCamera);
		CameraControllerComponent->OnThirdPersonCameraYawRotationDelegate.AddDynamic(this,
			&ThisClass::UG2ICharacterMovementComponent::SetCameraPendingYawRotation);
	}
}

void UG2ICharacterMovementComponent::PossessedByNewController(APawn *ChangedPawn)
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}
	
	if (ChangedPawn == Owner)
	{
		if (bIsCrouchState)
		{
			Owner->Crouch();
		}
	}
}

bool UG2ICharacterMovementComponent::CanUncrouch() const
{
	// Copying logic default Unreal function:
	// void UCharacterMovementComponent::UnCrouch(bool bClientSimulation)
	// Part of the logic that checks if default owner's capsule size fits (no encroachment)
	
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return false;
	}

	UClass *ClassOwner = Owner->GetClass();
	if (!ensure(ClassOwner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner hasn't class in %s"), *GetName());
		return false;
	}
	
	ACharacter *DefaultCharacter = ClassOwner->GetDefaultObject<ACharacter>();
	if (!ensure(DefaultCharacter))
	{
		UE_LOG(LogG2I, Error, TEXT("DefaultCharacter in logic of void UCharacterMovementComponent::UnCrouch("
							 "bool bClientSimulation) in %s"), *GetName());
		return false;
	}

	UCapsuleComponent *OwnerCapsuleComponent = Owner->GetCapsuleComponent();
	if (!ensure(OwnerCapsuleComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("%s hasn't capsule collision component in %s"), *Owner->GetName(),
			*GetName());
		return false;
	}

	UCapsuleComponent *DefaultCharacterCapsuleComponent = DefaultCharacter->GetCapsuleComponent();
	if (!ensure(DefaultCharacterCapsuleComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("%s hasn't capsule collision component in %s"), *DefaultCharacter->GetName(),
			*GetName());
		return false;
	}
	
	
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return false;
	}
	
	const float CurrentCrouchedHalfHeight = OwnerCapsuleComponent->GetScaledCapsuleHalfHeight();
	const float ComponentScale = OwnerCapsuleComponent->GetShapeScale();
	const float OldUnscaledHalfHeight = OwnerCapsuleComponent->GetUnscaledCapsuleHalfHeight();
	const float HalfHeightAdjust =
		DefaultCharacterCapsuleComponent->GetUnscaledCapsuleHalfHeight() - OldUnscaledHalfHeight;
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

					
	// Try to stay in place and see if the larger capsule fits. We use a slightly taller capsule to avoid penetration.
	constexpr float SweepInflation = UE_KINDA_SMALL_NUMBER * 10.f;
	FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, Owner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(CapsuleParams, ResponseParam);

	// Compensate for the difference between current capsule size and standing size

	// Compensate for the difference between current capsule size and standing size
	const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom,
		-SweepInflation - ScaledHalfHeightAdjust); // Shrink by negative amount, so actually grow it.
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
	bool bEncroached = true;

	if (!bCrouchMaintainsBaseLocation)
	{
		// Expand in place
		bEncroached = World->OverlapBlockingTestByChannel(PawnLocation, GetWorldToGravityTransform(),
			CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
	
		if (bEncroached)
		{
			// Try adjusting capsule position to see if we can avoid encroachment.
			if (ScaledHalfHeightAdjust > 0.f)
			{
				// Shrink to a short capsule, sweep down to base to find where that would hit something,
				// and then try to stand up from there.
				float PawnRadius, PawnHalfHeight;
				OwnerCapsuleComponent->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);
				const float ShrinkHalfHeight = PawnHalfHeight - PawnRadius;
				const float TraceDist = PawnHalfHeight - ShrinkHalfHeight;
				const FVector Down = TraceDist * GetGravityDirection();

				FHitResult Hit(1.f);
				const FCollisionShape ShortCapsuleShape =
					GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, ShrinkHalfHeight);

				if (Hit.bStartPenetrating)
				{
					bEncroached = true;
				}
				else
				{
					// Compute where the base of the sweep ended up, and see if we can stand there
					const float DistanceToBase = (Hit.Time * TraceDist) + ShortCapsuleShape.Capsule.HalfHeight;
					const FVector Adjustment = (-DistanceToBase + StandingCapsuleShape.Capsule.HalfHeight +
						SweepInflation + MIN_FLOOR_DIST / 2.f) * -GetGravityDirection();
					const FVector NewLoc = PawnLocation + Adjustment;
					bEncroached = World->OverlapBlockingTestByChannel(NewLoc, GetWorldToGravityTransform(),
						CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
				}
			}
		}
	}
	else
	{
		// Expand while keeping base location the same.
		FVector StandingLocation = PawnLocation + (StandingCapsuleShape.GetCapsuleHalfHeight() -
			CurrentCrouchedHalfHeight) * -GetGravityDirection();
		bEncroached = World->OverlapBlockingTestByChannel(StandingLocation, GetWorldToGravityTransform(),
			CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

		if (bEncroached)
		{
			if (IsMovingOnGround())
			{
				// Something might be just barely overhead, try moving down closer to the floor to avoid it.
				constexpr float MinFloorDist = UE_KINDA_SMALL_NUMBER * 10.f;
				if (CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > MinFloorDist)
				{
					StandingLocation -= (CurrentFloor.FloorDist - MinFloorDist) * -GetGravityDirection();
					bEncroached = World->OverlapBlockingTestByChannel(StandingLocation,
						GetWorldToGravityTransform(), CollisionChannel, StandingCapsuleShape, CapsuleParams,
						ResponseParam);
				}
			}				
		}
	}
						
	return !bEncroached;
}

void UG2ICharacterMovementComponent::EnableRotationTowardsCamera()
{
	bWantsRotationTowardsCamera = true;
	
	if (!bCanRotationTowardsCamera)
	{
		return;
	}
	
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}
	
	Owner->bUseControllerRotationPitch = true;
	Owner->bUseControllerRotationYaw = true;
	Owner->bUseControllerRotationRoll = true;
}

void UG2ICharacterMovementComponent::DisableRotationTowardsCamera()
{
	bWantsRotationTowardsCamera = false;
	
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}
	
	Owner->bUseControllerRotationPitch = false;
	Owner->bUseControllerRotationYaw = false;
	Owner->bUseControllerRotationRoll = false;
}

void UG2ICharacterMovementComponent::SetMovementWithThirdPersonCamera(const EG2ICameraBlendState CurrentBlendState,
	const UCameraComponent* NewCamera)
{
	switch (CurrentBlendState)
	{
	case EG2ICameraBlendState::Start:
		if (!ensure(World))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(), *UWorld::StaticClass()->GetName());
		}
		else
		{
			World->GetTimerManager().ClearTimer(TimerBeforeDisableRotationTowardsCamera);
		}
		ResetCameraPendingYawRotation(NewCamera);
		return;
	case EG2ICameraBlendState::Finish:
		bCanRotationTowardsCamera = true;
		if (bWantsRotationTowardsCamera)
		{
			EnableRotationTowardsCamera();
		}
		return;
	}
}

void UG2ICharacterMovementComponent::SetMovementWithFixedCamera(const EG2ICameraBlendState CurrentBlendState,
	const UCameraComponent* NewCamera)
{
	if (CurrentBlendState == EG2ICameraBlendState::Start)
	{
		bCanRotationTowardsCamera = false;
		DisableRotationTowardsCamera();
		
		if (!ensure(World))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(), *UWorld::StaticClass()->GetName());
			ResetCameraPendingYawRotation(NewCamera);
			return;
		}

		World->GetTimerManager().ClearTimer(TimerBeforeDisableRotationTowardsCamera);
		
		if (!ensure(CameraDefaultsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
				*UG2ICameraDefaultsParameters::StaticClass()->GetName());
			ResetCameraPendingYawRotation(NewCamera);
			return;
		}
		
		const FTimerDelegate Delegate = FTimerDelegate::CreateUObject(
			this, &ThisClass::ResetCameraPendingYawRotation, NewCamera);
		World->GetTimerManager().SetTimer(TimerBeforeDisableRotationTowardsCamera, Delegate,
			CameraDefaultsParameters->PendingTimeAfterSwitchingToControlCharacter, false);
	}
}

void UG2ICharacterMovementComponent::ResetCameraPendingYawRotation(const UCameraComponent* NewCamera)
{
	CameraPendingYawRotation = 0.;
	
	if (!ensure(NewCamera))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Attempt to set null new camera"), *GetName());
		return;
	}
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
			*GetName(), *AG2IPlayerController::StaticClass()->GetName());
		return;
	}
	PlayerController->SetRotationTowardsCamera(*NewCamera);
}

void UG2ICharacterMovementComponent::SetCameraPendingYawRotation(const double YawValue)
{
	CameraPendingYawRotation = YawValue;
}
