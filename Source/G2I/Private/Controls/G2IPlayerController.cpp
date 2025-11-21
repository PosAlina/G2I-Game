#include "G2IPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "G2I.h"
#include "G2IReactToInputInterface.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"

void AG2IPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetupInputComponent();
}

void AG2IPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

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

				EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
			}
			else
			{
				UE_LOG(LogG2I, Log, TEXT("Player Controller \"%s\" used not Enhanced Input Component"), *GetNameSafe(this));
			}
		}
		else
		{
			UE_LOG(LogG2I, Log, TEXT("Local character is not defined"))
		}
	}
}

void AG2IPlayerController::Look(const FInputActionValue& Value)
{
	if (APawn *CurrentCharacter = GetPawn())
	{
		if (CurrentCharacter->Implements<UG2IReactToInputInterface>())
		{
			const FVector2D LookAxisVector = Value.Get<FVector2D>();
			const float Yaw = LookAxisVector.X;
			const float Pitch = LookAxisVector.Y;
			IG2IReactToInputInterface::Execute_LookAction(CurrentCharacter, Yaw, Pitch);
		}
		else
		{
			UE_LOG(LogG2I, Log, TEXT("Current local character is not implemented %s interface for definition action"), *UG2IReactToInputInterface::StaticClass()->GetName())
		}
	}
	else
	{
		UE_LOG(LogG2I, Log, TEXT("Current local character is not implemented %s interface for definition action"), *UG2IReactToInputInterface::StaticClass()->GetName())
	}
}

void AG2IPlayerController::MouseLook(const FInputActionValue& Value)
{
	if (APawn *CurrentCharacter = GetPawn())
	{
		if (CurrentCharacter->Implements<UG2IReactToInputInterface>())
		{
			const FVector2D LookAxisVector = Value.Get<FVector2D>();
			const float Yaw = LookAxisVector.X;
			const float Pitch = LookAxisVector.Y;
			IG2IReactToInputInterface::Execute_LookAction(CurrentCharacter, Yaw, Pitch);
		}
		else
		{
			UE_LOG(LogG2I, Log, TEXT("Current local character is not implemented %s interface for definition action"), *UG2IReactToInputInterface::StaticClass()->GetName())
		}
	}
	else
	{
		UE_LOG(LogG2I, Log, TEXT("Current local character is not implemented %s interface for definition action"), *UG2IReactToInputInterface::StaticClass()->GetName())
	}
}

void AG2IPlayerController::Move(const FInputActionValue& Value)
{
	if (APawn *CurrentCharacter = GetPawn())
	{
		if (CurrentCharacter->Implements<UG2IReactToInputInterface>())
		{
			const FVector2D MovementVector = Value.Get<FVector2D>();
			const float Right = MovementVector.X;
			const float Forward = MovementVector.Y;
			const FRotator Rotation = GetControlRotation();
			IG2IReactToInputInterface::Execute_MoveAction(CurrentCharacter, Right, Forward, Rotation);
		}
		else
		{
			UE_LOG(LogG2I, Log, TEXT("Current local character is not implemented %s interface for definition action"), *UG2IReactToInputInterface::StaticClass()->GetName())
		}
	}
	else
	{
		UE_LOG(LogG2I, Log, TEXT("Current local character is not implemented %s interface for definition action"), *UG2IReactToInputInterface::StaticClass()->GetName())
	}
}

void AG2IPlayerController::Jump(const FInputActionValue& Value)
{
	if (APawn *CurrentCharacter = GetPawn())
	{
		if (CurrentCharacter->Implements<UG2IReactToInputInterface>())
		{
			IG2IReactToInputInterface::Execute_JumpAction(CurrentCharacter);
		}
		else
		{
			UE_LOG(LogG2I, Log, TEXT("Current local character is not implemented %s interface for definition action"), *UG2IReactToInputInterface::StaticClass()->GetName())
		}
	}
	else
	{
		UE_LOG(LogG2I, Log, TEXT("Current local character is not implemented %s interface for definition action"), *UG2IReactToInputInterface::StaticClass()->GetName())
	}
}

void AG2IPlayerController::StopJumping(const FInputActionValue& Value)
{
	if (APawn *CurrentCharacter = GetPawn())
	{
		if (CurrentCharacter->Implements<UG2IReactToInputInterface>())
		{
			IG2IReactToInputInterface::Execute_StopJumpingAction(CurrentCharacter);
		}
		else
		{
			UE_LOG(LogG2I, Log, TEXT("Current local character is not implemented %s interface for definition action"), *UG2IReactToInputInterface::StaticClass()->GetName())
		}
	}
	else
	{
		UE_LOG(LogG2I, Log, TEXT("Current local character is not implemented %s interface for definition action"), *UG2IReactToInputInterface::StaticClass()->GetName())
	}
}