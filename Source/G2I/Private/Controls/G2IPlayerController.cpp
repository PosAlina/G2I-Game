#include "G2IPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "G2I.h"
#include "G2IPlayerCameraManager.h"
#include "Camera/G2IThirdPersonCameraInputInterface.h"
#include "G2IPlayerState.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Components/G2IInteractionComponent.h"
#include "Components/G2ICharacterMovementComponent.h"
#include "Components/Camera/G2ICameraControllerComponent.h"
#include "GameFramework/Pawn.h"

void AG2IPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	bAutoManageActiveCameraTarget = false;
	if (IsLocalPlayerController())
	{
		if (const ULocalPlayer *LocalPlayer = GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				for (const UInputMappingContext* CurrentContext : InputMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
			else
			{
				UE_LOG(LogG2I, Log, TEXT("Player Controller \"%s\" has not any mapping context"), *GetNameSafe(this));
			}

			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
			{
				EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);

				EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);
				EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ThisClass::StopJumping);

				EnhancedInputComponent->BindAction(ToggleCrouchAction, ETriggerEvent::Started, this, &ThisClass::ToggleCrouch);

				EnhancedInputComponent->BindAction(SwitchCameraBehaviorAction, ETriggerEvent::Started, this,
					&ThisClass::SwitchCameraBehavior);
				
				EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

				EnhancedInputComponent->BindAction(SelectNextCharacterAction, ETriggerEvent::Started, this,
					&ThisClass::SelectNextCharacter);

				for (const auto& InteractAction : InteractActions) {
					EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ThisClass::Interact);
				}
			}
			else
			{
				UE_LOG(LogG2I, Log, TEXT("Player Controller \"%s\" used not Enhanced Input Component"), *GetNameSafe(this));
			}
		}
		else
		{
			UE_LOG(LogG2I, Log, TEXT("Local character is not defined"));
		}
	}
}

AG2IPlayerController::AG2IPlayerController()
{
	PlayerCameraManagerClass = AG2IPlayerCameraManager::StaticClass();
}

void AG2IPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);

	SetupCharacterActorComponents();
	SetupCamera();
}

void AG2IPlayerController::OnUnPossess()
{
	// Differs from the default function by the absence of setting of the view target in Player Controller
	if (APawn *CurrentPawn = GetPawn())
	{
		if (GetLocalRole() == ROLE_Authority)
		{
			CurrentPawn->SetReplicates(true);
		}
		CurrentPawn->UnPossessed();
	}
	SetPawn(nullptr);
}

void AG2IPlayerController::SetViewTargetWithBlend(class AActor* NewViewTarget, float BlendTime,
	enum EViewTargetBlendFunction BlendFunc, float BlendExp, bool bLockOutgoing)
{
	if (GetViewTarget() != this)
	{
		Super::SetViewTargetWithBlend(NewViewTarget, BlendTime, BlendFunc, BlendExp, bLockOutgoing);
	}
	else
	{
		SetViewTarget(NewViewTarget);
	}
}

void AG2IPlayerController::SetRotationTowardsCamera(const UCameraComponent& Camera)
{
	FVector NewCameraForwardVector = Camera.GetForwardVector();
	NewCameraForwardVector.Z = 0.0f;
	const FRotator NewControlRotator = NewCameraForwardVector.Rotation();
	SetControlRotation(NewControlRotator);
}

TObjectPtr<UG2ICameraDefaultsParameters> AG2IPlayerController::GetCameraDefaultsParameters()
{
	return CameraDefaultsParameters;
}

void AG2IPlayerController::SetupCharacterActorComponents()
{
	ThirdPersonCameraComponents.Empty();
	MovementComponents.Empty();
	InteractionComponents.Empty();
	
	if (const APawn *CurrentCharacter = GetPawn())
	{
		TSet<UActorComponent*> CharacterComponents = CurrentCharacter->GetComponents();
		for (UActorComponent *Component : CharacterComponents)
		{
			if (Component->Implements<UG2ICameraControllerInputInterface>())
			{
				CameraControllersComponent = Component;
			}

			if (Component->Implements<UG2IThirdPersonCameraInputInterface>())
			{
				ThirdPersonCameraComponents.Add(Component);
			}
			
			if (Component->Implements<UG2IMovementInputInterface>())
			{
				MovementComponents.Add(Component);
			}

			if (Component->Implements<UG2IInteractionInputInterface>())
			{
				InteractionComponents.Add(Component);
			}
		}
	}
	else
	{
		UE_LOG(LogG2I, Log, TEXT("Local character is not defined"));
	}
}

void AG2IPlayerController::SetupCamera()
{
	if (!ensure(CameraControllersComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller %s hasn't camera controller component"), *GetName());
		return;
	}
	
	if (!ensure(CameraControllersComponent->Implements<UG2ICameraControllerInputInterface>()))
	{
		UE_LOG(LogG2I, Error, TEXT("Camera controller component %s doesn't implement camera controller"
							 "interface in %s"), *CameraControllersComponent->GetName(), *GetName());
		return;
	}
	
	IG2ICameraControllerInputInterface::Execute_SetupCurrentCamera(CameraControllersComponent);
}

void AG2IPlayerController::SwitchCameraBehavior()
{
	if (!ensure(CameraControllersComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller %s hasn't camera controller component"), *GetName());
		return;
	}
	
	if (!ensure(CameraControllersComponent->Implements<UG2ICameraControllerInputInterface>()))
	{
		UE_LOG(LogG2I, Error, TEXT("Camera controller component %s doesn't implement camera controller"
							 "interface in %s"), *CameraControllersComponent->GetName(), *GetName());
		return;
	}

	IG2ICameraControllerInputInterface::Execute_SwitchCameraBehavior(CameraControllersComponent);
}

void AG2IPlayerController::Look(const FInputActionValue& Value)
{
	for (UActorComponent *Component : ThirdPersonCameraComponents)
	{
		if (Component->Implements<UG2IThirdPersonCameraInputInterface>())
		{
			const FVector2D LookAxisVector = Value.Get<FVector2D>();
			const float Yaw = LookAxisVector.X;
			const float Pitch = LookAxisVector.Y;
			IG2IThirdPersonCameraInputInterface::Execute_LookAction(Component, Yaw, Pitch);
		}
	}
}

void AG2IPlayerController::Move(const FInputActionValue& Value)
{
	for (UActorComponent *Component : MovementComponents)
	{
		if (Component->Implements<UG2IMovementInputInterface>())
		{
			const FVector2D MovementVector = Value.Get<FVector2D>();
			const float Right = MovementVector.X;
			const float Forward = MovementVector.Y;
			const FRotator Rotation = GetControlRotation();
			IG2IMovementInputInterface::Execute_MoveAction(Component, Right, Forward, Rotation);
		}
		else
		{
			UE_LOG(LogG2I, Warning, TEXT("In Movement Components array %s contains component which not "
								"implemented needed interface"), *Component->GetName());
		}
	}
}

void AG2IPlayerController::Jump(const FInputActionValue& Value)
{
	for (UActorComponent *Component : MovementComponents)
	{
		if (Component->Implements<UG2IMovementInputInterface>())
		{
			IG2IMovementInputInterface::Execute_JumpAction(Component);
		}
		else
		{
			UE_LOG(LogG2I, Warning, TEXT("In Movement Components array %s contains component which not "
								"implemented needed interface"), *Component->GetName());
		}
	}
}

void AG2IPlayerController::StopJumping(const FInputActionValue& Value)
{
	for (UActorComponent *Component : MovementComponents)
	{
		if (Component->Implements<UG2IMovementInputInterface>())
		{
			IG2IMovementInputInterface::Execute_StopJumpingAction(Component);
		}
		else
		{
			UE_LOG(LogG2I, Warning, TEXT("In Movement Components array %s contains component which not "
								"implemented needed interface"), *Component->GetName());
		}
	}
}

void AG2IPlayerController::ToggleCrouch(const FInputActionValue& Value)
{
	for (UActorComponent *Component : MovementComponents)
	{
		if (Component->Implements<UG2IMovementInputInterface>())
		{
			IG2IMovementInputInterface::Execute_ToggleCrouchAction(Component);
		}
		else
		{
			UE_LOG(LogG2I, Warning, TEXT("In Movement Components array %s contains component which not "
								"implemented needed interface"), *Component->GetName());
		}
	}
}

void AG2IPlayerController::SelectNextCharacter(const FInputActionValue& Value)
{
    if (AG2IPlayerState *CurrentPlayerState = GetPlayerState<AG2IPlayerState>())
    {
        CurrentPlayerState->SelectNextCharacter();
    }
    else
    {
        UE_LOG(LogG2I, Error, TEXT("This player state %s is not class %s"),
            *GetName(), *AG2IPlayerState::StaticClass()->GetName());
    }
}

void AG2IPlayerController::Interact(const FInputActionInstance& Instance)
{
	for (UActorComponent* Component : InteractionComponents)
	{
		if (Component->Implements<UG2IInteractionInputInterface>())
		{
			if (const UInputAction* Action = Instance.GetSourceAction()) {
				if (ActionToTagMap.Contains(Action))
				{
					FName Tag = ActionToTagMap[Action];
					IG2IInteractionInputInterface::Execute_InteractAction(Component, Tag);
				}
				else {
					UE_LOG(LogG2I, Log, TEXT("Map doesn't contains interact action"));
				}
			}
			else {
				UE_LOG(LogG2I, Log, TEXT("Can't get interact action for this local character"));
			}
			
		}
		else
		{
			UE_LOG(LogG2I, Warning, TEXT("In Interaction Components array %s contains component which not "
				"implemented needed interface"), *Component->GetName());
		}
	}
}
