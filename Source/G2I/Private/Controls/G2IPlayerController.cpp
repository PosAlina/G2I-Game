#include "G2IPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "G2I.h"
#include "G2IAimingInterface.h"
#include "G2IPlayerCameraManager.h"
#include "G2IThirdPersonCameraInputInterface.h"
#include "G2IGlovePunchInterface.h"
#include "G2IPlayerState.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "G2IInteractionComponent.h"
#include "G2ICharacterMovementComponent.h"
#include "G2ICameraControllerComponent.h"
#include "G2IFlightInterface.h"
#include "G2IGameInstance.h"
#include "GameFramework/Pawn.h"
#include "G2ISteamMovementInputInterface.h"
#include "G2ISteamShotInputInterface.h"
#include "G2IUIManager.h"
#include "Kismet/KismetSystemLibrary.h"

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
					InputKeyMappings.Append(CurrentContext->GetMappings());
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
				EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::FlyUp);
				EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ThisClass::StopJumping);
				EnhancedInputComponent->BindAction(FlightDownAction, ETriggerEvent::Triggered, this, &ThisClass::FlyDown);
				EnhancedInputComponent->BindAction(FlightDownAction, ETriggerEvent::Completed, this, &ThisClass::StopJumping);
				
				//EnhancedInputComponent->BindAction(ToggleCrouchAction, ETriggerEvent::Started, this, &ThisClass::ToggleCrouch);

				EnhancedInputComponent->BindAction(SwitchCameraBehaviorAction, ETriggerEvent::Started, this,
					&ThisClass::SwitchCameraBehavior);
				
				EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

				EnhancedInputComponent->BindAction(SelectNextCharacterAction, ETriggerEvent::Started, this,
					&ThisClass::SelectNextCharacter);

				for (const auto& InteractAction : InteractActions) {
					EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ThisClass::Interact);
				}

				EnhancedInputComponent->BindAction(TakeAimAction, ETriggerEvent::Started, this, &ThisClass::StartAiming);
				EnhancedInputComponent->BindAction(TakeAimAction, ETriggerEvent::Completed, this, &ThisClass::StopAiming);

				EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ThisClass::Shoot);

				EnhancedInputComponent->BindAction(ToggleFollowAIBehindPlayerAction, ETriggerEvent::Started,
					this, &ThisClass::ToggleFollowAIBehindPlayer);

				EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started,this, &ThisClass::CallPause);

				EnhancedInputComponent->BindAction(GlovePunchAction, ETriggerEvent::Started, this, &ThisClass::GlovePunchActivation);
#if WITH_EDITOR
				EnhancedInputComponent->BindAction(SaveAction, ETriggerEvent::Triggered, this, &ThisClass::SaveGameplay);
				EnhancedInputComponent->BindAction(LoadAction, ETriggerEvent::Triggered, this, &ThisClass::LoadGameplay);
#endif
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

	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}
	const UG2IGameInstance *GameInstance = Cast<UG2IGameInstance>(World->GetGameInstance());
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("Game Instance doesn't exist in %s"), *GetName());
		return;
	}
	UIManager = GameInstance->GetSubsystem<UG2IUIManager>();
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s isn't defined in %s"),
			*UG2IUIManager::StaticClass()->GetName(), *GetName());
	}
	UIManager->OnPlayerControllerInitDelegate.Broadcast(this);
}

AG2IPlayerController::AG2IPlayerController()
{
	PlayerCameraManagerClass = AG2IPlayerCameraManager::StaticClass();

	FGameplayTag SavableTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Savable"));

	if (SavableTag.IsValid())
	{
		GameplayTags.AddTag(SavableTag);
	}
}

void AG2IPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);

	if (NewPawn && GetPawn() == NewPawn)
	{
		OnPossessPawnDelegate.Broadcast(NewPawn);
	}
	
	SetupCharacterActorComponents();
	SetupCamera();
}

void AG2IPlayerController::OnUnPossess()
{
	APawn *CurrentPawn = GetPawn();
	if (CurrentPawn)
	{
		if (GetLocalRole() == ROLE_Authority)
		{
			CurrentPawn->SetReplicates(true);
		}
		CurrentPawn->UnPossessed();
	}
	SetPawn(nullptr);

	if (CurrentPawn && !GetPawn())
	{
		OnUnPossessPawnDelegate.Broadcast(CurrentPawn);
	}
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

void AG2IPlayerController::CallPause(const FInputActionValue& Value)
{
	SetPause(true);
}

bool AG2IPlayerController::SetPause(bool bPause, FCanUnpause CanUnpauseDelegate)
{
	if (!Super::SetPause(bPause, CanUnpauseDelegate))
	{
		return false;
	}

	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s isn't defined in %s"),
			*UG2IUIManager::StaticClass()->GetName(), *GetName());
		return true;
	}
	if (bPause)
	{
		UIManager->OpenPauseWidget();
	}
	else
	{
		UIManager->ClosePauseWidget();
	}
	
	return true;
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

void AG2IPlayerController::QuitGame()
{
	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}
	
	UKismetSystemLibrary::QuitGame(World, this, EQuitPreference::Quit, true);
}

FName AG2IPlayerController::GetKeyName(UInputAction* InputAction)
{
	for (const FEnhancedActionKeyMapping& Mapping : InputKeyMappings)
	{
		if (Mapping.Action == InputAction)
		{
			return Mapping.Key.GetFName();
		}
	}
	return NAME_None;
}

TMap<TObjectPtr<UInputAction>, FName>& AG2IPlayerController::GetActionToTagMap()
{
	return ActionToTagMap;
}

void AG2IPlayerController::SaveData_Implementation(UG2IGameplaySaveGame* SaveGameRef)
{
	if (SaveGameRef && GetPawn())
	{
		SaveGameRef->PlayersSaveData.CurrentCharacter = GetPawn()->GetName();
		UE_LOG(LogG2I, Log, TEXT("PlayerController: %s saved."), *SaveGameRef->PlayersSaveData.CurrentCharacter);
	}
}

void AG2IPlayerController::LoadData_Implementation(const UG2IGameplaySaveGame* SaveGameRef)
{
	if (SaveGameRef)
		if (const APawn* CurrentCharacter = GetPawn())
		{
			if (!CurrentCharacter->GetName().Equals(SaveGameRef->PlayersSaveData.CurrentCharacter))
				SelectNextCharacter(FInputActionValue());
		}
}

void AG2IPlayerController::SetupCharacterActorComponents()
{
	ThirdPersonCameraComponents.Empty();
	InteractionComponents.Empty();
	MovementComponent = nullptr;
	SteamMovementComponent = nullptr;
	AimingComponent = nullptr;
	SteamShotComponent = nullptr;
	FlightComponent = nullptr;
	GlovePunchComponent = nullptr;
	
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
				MovementComponent = Component;
			}

			if (Component->Implements<UG2IInteractionInputInterface>())
			{
				InteractionComponents.Add(Component);
			}

			if (Component->Implements<UG2ISteamMovementInputInterface>())
			{
				SteamMovementComponent = Component;
			}

			if (Component->Implements<UG2IAimingInterface>())
			{
				AimingComponent = Component;
			}

			if (Component->Implements<UG2ISteamShotInputInterface>())
			{
				SteamShotComponent = Component;
			}

			if(Component->Implements<UG2IFlightInterface>())
			{
				FlightComponent = Component;
			}
			
			if (Component->Implements<UG2IGlovePunchInterface>())
			{
				GlovePunchComponent = Component;
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
	if (!ensure(MovementComponent))
	{
		UE_LOG(LogG2I, Warning, TEXT("Pawn doesn't have component with movement interface in %s"), *GetName());
		return;
	}
	
	if (MovementComponent->Implements<UG2IMovementInputInterface>())
	{
		const FVector2D MovementVector = Value.Get<FVector2D>();
		const float Right = MovementVector.X;
		const float Forward = MovementVector.Y;
		const FRotator Rotation = GetControlRotation();
		IG2IMovementInputInterface::Execute_MoveAction(MovementComponent, Right, Forward, Rotation);
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("%s does not implemented movement interface"),
			*MovementComponent->GetName());
	}
}

void AG2IPlayerController::FlyUp(const FInputActionValue& Value)
{
	Fly(1);
}

void AG2IPlayerController::FlyDown(const FInputActionValue& Value)
{
	Fly(-1);
}

void AG2IPlayerController::Fly(int Direction)
{
	if (FlightComponent && MovementComponent)
	{
		IG2IFlightInterface::Execute_Fly(FlightComponent, MovementComponent, Direction);
	}
	else
	{
		UE_LOG(LogG2I, Log, TEXT("Pawn doesn't have component with fly interface in %s"), *GetName());
		UE_LOG(LogG2I, Log, TEXT("Pawn doesn't have component with movement interface in %s"), *GetName());
	}
}

void AG2IPlayerController::Jump(const FInputActionValue& Value)
{
	if (!FlightComponent)
	{
		UE_LOG(LogG2I, Log, TEXT("Pawn doesn't have component with fly interface in %s"), *GetName());
	}
	else
	{
		return;
	}
	
	if (!ensure(MovementComponent))
	{
		UE_LOG(LogG2I, Warning, TEXT("Pawn doesn't have component with movement interface in %s"), *GetName());
		return;
	}
	
	if (MovementComponent->Implements<UG2IMovementInputInterface>())
	{
		if (!IG2IMovementInputInterface::Execute_IsInAir(MovementComponent))
		{
			IG2IMovementInputInterface::Execute_JumpAction(MovementComponent);
		}
		else
		{
			if (SteamMovementComponent && SteamMovementComponent->Implements<UG2ISteamMovementInputInterface>())
			{
				IG2ISteamMovementInputInterface::Execute_SteamJumpAction(SteamMovementComponent);
			}
		}
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("%s does not implemented movement interface"),
			*MovementComponent->GetName());
	}
}

void AG2IPlayerController::StopJumping(const FInputActionValue& Value)
{
	if (!FlightComponent)
	{
		UE_LOG(LogG2I, Log, TEXT("Pawn doesn't have component with fly interface in %s"), *GetName());
	}
	else
	{
		IG2IFlightInterface::Execute_StopFly(FlightComponent, MovementComponent);
		return;
	}
	
	if (!ensure(MovementComponent))
	{
		UE_LOG(LogG2I, Warning, TEXT("Pawn doesn't have component with movement interface in %s"), *GetName());
		return;
	}
	
	if (MovementComponent->Implements<UG2IMovementInputInterface>())
	{
		IG2IMovementInputInterface::Execute_StopJumpingAction(MovementComponent);
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("%s does not implemented movement interface"),
			*MovementComponent->GetName());
	}
}

void AG2IPlayerController::ToggleCrouch(const FInputActionValue& Value)
{
	if (!ensure(MovementComponent))
	{
		UE_LOG(LogG2I, Warning, TEXT("Pawn doesn't have component with movement interface in %s"), *GetName());
		return;
	}
	
	if (MovementComponent->Implements<UG2IMovementInputInterface>())
	{
		IG2IMovementInputInterface::Execute_ToggleCrouchAction(MovementComponent);
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("%s does not implemented movement interface"),
			*MovementComponent->GetName());
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

void AG2IPlayerController::StartAiming(const FInputActionValue& Value)
{
	if (AimingComponent && AimingComponent->Implements<UG2IAimingInterface>())
	{
		IG2IAimingInterface::Execute_StartAimingAction(AimingComponent);
	}
}

void AG2IPlayerController::StopAiming(const FInputActionValue& Value)
{
	if (AimingComponent && AimingComponent->Implements<UG2IAimingInterface>())
	{
		IG2IAimingInterface::Execute_StopAimingAction(AimingComponent);
	}
}

void AG2IPlayerController::Shoot(const FInputActionValue& Value)
{
	if (AimingComponent && AimingComponent->Implements<UG2IAimingInterface>())
	{
		if (!IG2IAimingInterface::Execute_IsAiming(AimingComponent))
		{
			return;
		}
		
		if (SteamShotComponent && SteamShotComponent->Implements<UG2ISteamShotInputInterface>()
			&& SteamShotComponent == IG2IAimingInterface::Execute_GetCurrentComponentUsingAim(AimingComponent))
		{
			const FG2IHitInfo AimLineHitInfo = IG2IAimingInterface::Execute_GetAimLineHitInfo(AimingComponent);
			IG2ISteamShotInputInterface::Execute_ShootAction(SteamShotComponent, AimLineHitInfo);
		}
	}
}

void AG2IPlayerController::ToggleFollowAIBehindPlayer(const FInputActionValue& Value)
{
	bIsFollowingAIBehindPlayer = !bIsFollowingAIBehindPlayer;
	OnToggleFollowAIBehindPlayerDelegate.Broadcast(bIsFollowingAIBehindPlayer);
}

void AG2IPlayerController::GlovePunchActivation(const FInputActionInstance& Instance)
{
	IG2IGlovePunchInterface::Execute_GlovePunchActivation(GlovePunchComponent);
}

void AG2IPlayerController::SaveGameplay(const FInputActionValue& Value)
{
	if (auto* GameInstance = GetGameInstance())
	{
		if (GameInstance->Implements<UG2ISaveGameplayInterface>())
			IG2ISaveGameplayInterface::Execute_SaveAllDataAndGameplay(GameInstance, true);
		else
			UE_LOG(LogG2I, Warning, TEXT("%s doesn't implement interface UG2ISaveGameplayInterface."), *GameInstance->GetName());
	}
}

void AG2IPlayerController::LoadGameplay(const FInputActionValue& Value)
{
	if (auto* GameInstance = GetGameInstance())
	{
		if (GameInstance->Implements<UG2ISaveGameplayInterface>())
			IG2ISaveGameplayInterface::Execute_LoadRequestedData(GameInstance, this);
		else
			UE_LOG(LogG2I, Warning, TEXT("%s doesn't implement interface UG2ISaveGameplayInterface."), *GameInstance->GetName());
	}
}
