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
#include "G2IWidgetNames.h"
#include "G2ISavingGameplayManager.h"
#include "Kismet/KismetSystemLibrary.h"

void FG2IInputMappingContexts::AddAllContextsToMapping(UEnhancedInputLocalPlayerSubsystem& Subsystem) const
{
	for (const UInputMappingContext *Context : Contexts)
	{
		if (!Context)
		{
			continue;
		}

		Subsystem.AddMappingContext(Context, 0);
	}
}

void FG2IInputMappingContexts::RemoveAllContextsToMapping(UEnhancedInputLocalPlayerSubsystem& Subsystem) const
{
	for (const UInputMappingContext *Context : Contexts)
	{
		if (!Context)
		{
			continue;
		}
		
		Subsystem.RemoveMappingContext(Context);
	}
}

void AG2IPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	bAutoManageActiveCameraTarget = false;
	
	SetupDefaults();
}

void AG2IPlayerController::SetupDefaults()
{
	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetActorNameOrLabel());
		return;
	}
	const UG2IGameInstance *GameInstance = Cast<UG2IGameInstance>(World->GetGameInstance());
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("Game Instance doesn't exist in %s"), *GetActorNameOrLabel());
		return;
	}
	UIManager = GameInstance->GetSubsystem<UG2IUIManager>();
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s isn't defined in %s"),
			*UG2IUIManager::StaticClass()->GetName(), *GetActorNameOrLabel());
	}
	
	SetupKeyMapping();
	SetupCommonInput();
	BindEnhancedDelegates();
	
	GameInstance->OnPlayerControllerInitDelegate.Broadcast();
}

void AG2IPlayerController::SetupKeyMapping()
{
	for (const auto& [PawnClass, _] : InputMappingContextsByPawn)
	{
		InputKeyMappings.Add(PawnClass);
	}
	for (const auto& [_,ContextsInfo] : CommonInputMappingContexts)
	{
		for (const UInputMappingContext *Context : ContextsInfo.Contexts)
		{
			if (!Context)
			{
				continue;
			}
			for (auto& [__, Mapping] : InputKeyMappings)
			{
				Mapping.Mappings.Append(Context->GetMappings());
			}
		}
	}
	for (const auto& [PawnClass, ContextsInfo] : InputMappingContextsByPawn)
	{
		for (const UInputMappingContext *Context : ContextsInfo.Contexts)
		{
			if (!Context)
			{
				continue;
			}
			InputKeyMappings[PawnClass].Mappings.Append(Context->GetMappings());
		}
	}
}

void AG2IPlayerController::SetupCommonInput()
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetSubsystem();
	if (!Subsystem)
	{
		return;
	}

	for (const auto& [_,ContextsInfo] : CommonInputMappingContexts)
	{
		ContextsInfo.AddAllContextsToMapping(*Subsystem);
	}

#if WITH_EDITOR
	for (const auto& [_,DebugContextsInfo] : DebugCommonInputMappingContexts)
	{
		DebugContextsInfo.AddAllContextsToMapping(*Subsystem);
	}
#endif
}

void AG2IPlayerController::BindEnhancedDelegates()
{
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!ensure(EnhancedInputComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Input Component isn't Enhanced"), *GetActorNameOrLabel());
		return;
	}
	
	EnhancedInputComponent->BindAction(
		MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);

	EnhancedInputComponent->BindAction(
		JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);
	EnhancedInputComponent->BindAction(
		JumpAction, ETriggerEvent::Completed, this, &ThisClass::StopJumping);

	EnhancedInputComponent->BindAction(
		FlightUpAction, ETriggerEvent::Triggered, this, &ThisClass::FlyUp);
	EnhancedInputComponent->BindAction(
		FlightUpAction, ETriggerEvent::Completed, this, &ThisClass::StopFlight);
	EnhancedInputComponent->BindAction(
		FlightDownAction, ETriggerEvent::Triggered, this, &ThisClass::FlyDown);
	EnhancedInputComponent->BindAction(
		FlightDownAction, ETriggerEvent::Completed, this, &ThisClass::StopFlight);
	
	EnhancedInputComponent->BindAction(
		LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

	EnhancedInputComponent->BindAction(
		SelectNextCharacterAction, ETriggerEvent::Started, this, &ThisClass::SelectNextCharacter);

	for (const auto& InteractAction : InteractActions) {
		EnhancedInputComponent->BindAction(
			InteractAction, ETriggerEvent::Started, this, &ThisClass::Interact);
	}

	EnhancedInputComponent->BindAction(
		TakeAimAction, ETriggerEvent::Started, this, &ThisClass::StartAiming);
	EnhancedInputComponent->BindAction(
		TakeAimAction, ETriggerEvent::Completed, this, &ThisClass::StopAiming);

	EnhancedInputComponent->BindAction(
		ShootAction, ETriggerEvent::Started, this, &ThisClass::Shoot);

	EnhancedInputComponent->BindAction(
		ToggleFollowAIBehindPlayerAction, ETriggerEvent::Started, this, &ThisClass::ToggleFollowAIBehindPlayer);

	EnhancedInputComponent->BindAction(
		GlovePunchAction, ETriggerEvent::Started, this, &ThisClass::GlovePunchActivation);

#if WITH_EDITOR
	EnhancedInputComponent->BindAction(
	DebugPauseAction, ETriggerEvent::Started,this, &ThisClass::CallPause);
#else
	EnhancedInputComponent->BindAction(
		PauseAction, ETriggerEvent::Started, this, &ThisClass::CallPause);
#endif
	
#if WITH_EDITOR
	EnhancedInputComponent->BindAction(
		ToggleDebugAction, ETriggerEvent::Started, this, &ThisClass::ToggleDebugMode);
	EnhancedInputComponent->BindAction(
		ToggleCrouchAction, ETriggerEvent::Started, this, &ThisClass::ToggleCrouch);
	EnhancedInputComponent->BindAction(
		SwitchCameraBehaviorAction, ETriggerEvent::Started, this, &ThisClass::SwitchCameraBehavior);
	EnhancedInputComponent->BindAction(
		SaveAction, ETriggerEvent::Triggered, this, &ThisClass::SaveGameplay);
	EnhancedInputComponent->BindAction(
		LoadAction, ETriggerEvent::Triggered, this, &ThisClass::LoadGameplay);
#endif
}

UEnhancedInputLocalPlayerSubsystem* AG2IPlayerController::GetSubsystem() const
{
	const ULocalPlayer *LocalPlayer = GetLocalPlayer();
	if (!ensure(LocalPlayer))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Local character is null"), *GetActorNameOrLabel());
		return nullptr;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	if (!ensure(Subsystem))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't used Enhanced Input System"), *GetActorNameOrLabel());
		return nullptr;
	}
	return Subsystem;
}

TSubclassOf<APawn> AG2IPlayerController::GetPawnClass(const APawn* NewPawn) const
{
	if (!NewPawn)
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Attempt to set input for null pawn"), *GetActorNameOrLabel());
		return nullptr;
	}

	const TSubclassOf<APawn> PawnClass = NewPawn->GetClass();
	if (!ensure(PawnClass))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Pawn %s - Class is not ACharacter Class"), *GetActorNameOrLabel(),
			*NewPawn->GetActorNameOrLabel());
		return nullptr;
	}
	return PawnClass;
}

TSubclassOf<APawn> AG2IPlayerController::GetCurrentPawnClass() const
{
	return GetPawnClass(GetPawn());
}

UG2IAimingComponent* AG2IPlayerController::GetAimingComponent() const
{
	return Cast<UG2IAimingComponent>(AimingComponent);
}

TMap<TSubclassOf<APawn>, FG2IInputKeyMapping>& AG2IPlayerController::GetInputKeyMapping()
{
	return InputKeyMappings;
}

UInputAction* AG2IPlayerController::GetMoveAction()
{
	return MoveAction;
}

UInputAction* AG2IPlayerController::GetMoveSliderAction()
{
	return MoveSliderAction;
}

bool AG2IPlayerController::IsCurrentPawnClass(const TSubclassOf<APawn>& PawnClass) const
{
	return PawnClass == GetCurrentPawnClass();
}

void AG2IPlayerController::SetupInputForPawn(const APawn* NewPawn)
{
	const TSubclassOf<APawn>& PawnClass = GetPawnClass(NewPawn);
	UEnhancedInputLocalPlayerSubsystem *Subsystem = GetSubsystem();
	
	if (!Subsystem || !PawnClass)
	{
		return;
	}

	if (const FG2IInputMappingContexts *ContextsInfoForOverriden =
		InputMappingContextsForOverridingByPawn.Find(PawnClass))
	{
		SetupInputOverridenForPawn(*ContextsInfoForOverriden, *Subsystem);
	}
	else
	{
		SetupInputWithoutOverridenForPawn(PawnClass, *Subsystem);
	}
}

void AG2IPlayerController::SetupInputIfPawnClassIsCurrent(const TSubclassOf<APawn>& PawnClass)
{
	if (!ensure(PawnClass))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: PawnClass is null"), *GetActorNameOrLabel());
		return;
	}
	if (IsCurrentPawnClass(PawnClass))
	{
		SetupInputForPawn(GetPawn());
	}
}

void AG2IPlayerController::SetupInputOverridenForPawn(
	const FG2IInputMappingContexts& ContextsInfoForOverriden, UEnhancedInputLocalPlayerSubsystem& Subsystem)
{
	ContextsInfoForOverriden.AddAllContextsToMapping(Subsystem);
	if (const FG2IInputMappingContexts *ContextsInfo =
		CommonInputMappingContexts.Find(EG2IInputMappingContextType::Changeable))
	{
		ContextsInfo->RemoveAllContextsToMapping(Subsystem);
	}
#if WITH_EDITOR
	if (const FG2IInputMappingContexts *DebugContextsInfo =
		DebugCommonInputMappingContexts.Find(EG2IInputMappingContextType::Changeable))
	{
		DebugContextsInfo->RemoveAllContextsToMapping(Subsystem);
	}
#endif
}

void AG2IPlayerController::SetupInputWithoutOverridenForPawn(
	const TSubclassOf<APawn>& PawnClass, UEnhancedInputLocalPlayerSubsystem& Subsystem)
{
	if (const FG2IInputMappingContexts *ContextsInfo = InputMappingContextsByPawn.Find(PawnClass))
	{
		ContextsInfo->AddAllContextsToMapping(Subsystem);
	}
#if WITH_EDITOR
	if (const FG2IInputMappingContexts *DebugContextsInfo = DebugInputMappingContextsByPawn.Find(PawnClass))
	{
		DebugContextsInfo->AddAllContextsToMapping(Subsystem);
	}
#endif
}

void AG2IPlayerController::RemovedInputForPawn(const APawn* NewPawn)
{
	const TSubclassOf<APawn>& PawnClass = GetPawnClass(NewPawn);
	UEnhancedInputLocalPlayerSubsystem *Subsystem = GetSubsystem();
	
	if (!Subsystem || !PawnClass)
	{
		return;
	}

	if (const FG2IInputMappingContexts *ContextsInfoForOverriden =
		InputMappingContextsForOverridingByPawn.Find(PawnClass))
	{
		RemovedInputOverridenForPawn(*ContextsInfoForOverriden, *Subsystem);
	}
	else
	{
		RemovedInputWithoutOverridenForPawn(PawnClass, *Subsystem);
	}
}

void AG2IPlayerController::RemovedInputIfPawnClassIsCurrent(const TSubclassOf<APawn>& PawnClass)
{
	if (!ensure(PawnClass))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: PawnClass is null"), *GetActorNameOrLabel());
		return;
	}
	if (IsCurrentPawnClass(PawnClass))
	{
		RemovedInputForPawn(GetPawn());
	}
}

void AG2IPlayerController::RemovedInputOverridenForPawn(
	const FG2IInputMappingContexts& ContextsInfoForOverriden, UEnhancedInputLocalPlayerSubsystem& Subsystem)
{
	ContextsInfoForOverriden.RemoveAllContextsToMapping(Subsystem);
	if (const FG2IInputMappingContexts *ContextsInfo =
		CommonInputMappingContexts.Find(EG2IInputMappingContextType::Changeable))
	{
		ContextsInfo->AddAllContextsToMapping(Subsystem);
	}
#if WITH_EDITOR
	if (const FG2IInputMappingContexts *DebugContextsInfo =
		DebugCommonInputMappingContexts.Find(EG2IInputMappingContextType::Changeable))
	{
		DebugContextsInfo->AddAllContextsToMapping(Subsystem);
	}
#endif
}

void AG2IPlayerController::RemovedInputWithoutOverridenForPawn(
	const TSubclassOf<APawn>& PawnClass, UEnhancedInputLocalPlayerSubsystem& Subsystem)
{
	if (const FG2IInputMappingContexts *ContextsInfo = InputMappingContextsByPawn.Find(PawnClass))
	{
		ContextsInfo->RemoveAllContextsToMapping(Subsystem);
	}
#if WITH_EDITOR
	if (const FG2IInputMappingContexts *DebugContextsInfo = DebugInputMappingContextsByPawn.Find(PawnClass))
	{
		DebugContextsInfo->RemoveAllContextsToMapping(Subsystem);
	}
#endif
}

AG2IPlayerController::AG2IPlayerController()
{
	PlayerCameraManagerClass = AG2IPlayerCameraManager::StaticClass();
}

void AG2IPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);

	if (!NewPawn)
	{
		return;
	}
	
	if (GetPawn() != NewPawn)
	{
		return;
	}

	// TODO: SetupCharacterACtorComponents should be only once in start level, store in map with key - character class
	SetupCharacterActorComponents();
	SetupCamera();
	SetupInputForPawn(GetPawn());
	OnPossessPawnDelegate.Broadcast(NewPawn);
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

	if (!CurrentPawn)
	{
		return;
	}

	if (!GetPawn())
	{
		OnUnPossessPawnDelegate.Broadcast(CurrentPawn);
	}

	RemovedInputForPawn(CurrentPawn);
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
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s isn't defined in %s"),
			*UG2IUIManager::StaticClass()->GetName(), *GetName());
		return;
	}
	SetPause(true);
	
	UIManager->OpenWidget(EG2IWidgetNames::Pause);
	UIManager->SetupPauseWidget([this]()
	{
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
		SetPause(false);
	});
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

FName AG2IPlayerController::GetKeyName(UInputAction* InputAction, const TSubclassOf<APawn>& PawnClass)
{
	if (!ensure(PawnClass))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Attempt to find mapping for null class"), *GetActorNameOrLabel());
		return NAME_None;;
	}
	FG2IInputKeyMapping *Mappings = InputKeyMappings.Find(PawnClass);
	if (!ensure(Mappings))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Couldn't find input key mappings for character %s"), *GetActorNameOrLabel(),
			*PawnClass->GetName());
		return NAME_None;
	}
	
	for (const FEnhancedActionKeyMapping& Mapping : Mappings->Mappings)
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

void AG2IPlayerController::OverrideInputMappingContext(
	const TArray<TObjectPtr<UInputMappingContext>>& ContextsForOverride)
{
	const TSubclassOf<APawn>& PawnClass = GetCurrentPawnClass();
	if (!ensure(PawnClass))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: PawnClass is null"), *GetActorNameOrLabel());
		return;
	}
	RemovedInputIfPawnClassIsCurrent(PawnClass);
	const FG2IInputMappingContexts MappingContextForOverride = {ContextsForOverride};
	InputMappingContextsForOverridingByPawn.Add(PawnClass, MappingContextForOverride);
	SetupInputIfPawnClassIsCurrent(PawnClass);
}

void AG2IPlayerController::StopOverrideInputMappingContext()
{
	const TSubclassOf<APawn>& PawnClass = GetCurrentPawnClass();
	if (!ensure(PawnClass))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: PawnClass is null"), *GetActorNameOrLabel());
		return;
	}
	RemovedInputIfPawnClassIsCurrent(PawnClass);
	InputMappingContextsForOverridingByPawn.Remove(PawnClass);
	SetupInputIfPawnClassIsCurrent(PawnClass);
}

void AG2IPlayerController::OverrideInputMappingContextToSlider()
{
	OverrideInputMappingContext({SliderInputMappingContext});
}

void AG2IPlayerController::SetupCharacterActorComponents()
{
	ThirdPersonCameraComponents.Empty();
	InteractionComponents.Empty();
	MovementComponent = nullptr;
	AimingComponent = nullptr;
	SteamShotComponent = nullptr;
	FlightComponent = nullptr;
	GlovePunchComponent = nullptr;
#if WITH_EDITOR
	SteamMovementComponent = nullptr;
#endif
	
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

#if WITH_EDITOR
			if (Component->Implements<UG2ISteamMovementInputInterface>())
			{
				SteamMovementComponent = Component;
			}
#endif
		}
	}
	else
	{
		UE_LOG(LogG2I, Log, TEXT("Local character is not defined"));
	}
}

void AG2IPlayerController::SetupCamera() const
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

#if WITH_EDITOR
void AG2IPlayerController::ToggleDebugMode()
{
	bIsDebugAction = !bIsDebugAction;
	if (GEngine)
	{
		if (bIsDebugAction)
		{
			GEngine->AddOnScreenDebugMessage(-2, MAX_FLT, FColor::Silver, "DEBUG MODE: ON");
		}
		else
		{
			GEngine->RemoveOnScreenDebugMessage(-2);
		}
	}
}

void AG2IPlayerController::SwitchCameraBehavior()
{
	if (!bIsDebugAction)
	{
		return;
	}
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

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Silver, "DEBUG: Switch camera");
	}
	IG2ICameraControllerInputInterface::Execute_SwitchCameraBehavior(CameraControllersComponent);
}
#endif

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

void AG2IPlayerController::Fly(const int Direction) const
{
	if (!ensure(FlightComponent))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Pawn doesn't have flight component"), *GetActorNameOrLabel());
		return;
	}
	if (!ensure(FlightComponent->Implements<UG2IFlightInterface>()))
	{
		UE_LOG(LogG2I, Error, TEXT("%s does not implemented flight interface"),
			*FlightComponent->GetName());
		return;
	}
	
	if (Direction == 1 && IG2IFlightInterface::Execute_Fly(FlightComponent, Direction))
	{
		OnFlyUpDelegate.Broadcast();
	}
}

void AG2IPlayerController::StopFlight(const FInputActionValue& Value)
{
	if (!ensure(FlightComponent))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Pawn doesn't have flight component"), *GetActorNameOrLabel());
		return;
	}
	if (!ensure(FlightComponent->Implements<UG2IFlightInterface>()))
	{
		UE_LOG(LogG2I, Error, TEXT("%s does not implemented flight interface"),
			*FlightComponent->GetName());
		return;
	}
	
	IG2IFlightInterface::Execute_StopFly(FlightComponent);
}

void AG2IPlayerController::Jump(const FInputActionValue& Value)
{
	if (bIsNeedToSkipFirstJump)
	{
		bIsNeedToSkipFirstJump = false;
		return;
	}
	if (!ensure(MovementComponent))
	{
		UE_LOG(LogG2I, Warning, TEXT("Pawn doesn't have movement component in %s"), *GetName());
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
#if WITH_EDITOR
			if (!bIsDebugAction)
			{
				return;
			}
			if (SteamMovementComponent && SteamMovementComponent->Implements<UG2ISteamMovementInputInterface>())
			{
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Silver, "DEBUG: Steam jump");
				}
				IG2ISteamMovementInputInterface::Execute_SteamJumpAction(SteamMovementComponent);
			}
#endif
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

#if WITH_EDITOR
void AG2IPlayerController::ToggleCrouch()
{
	if (!bIsDebugAction)
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
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Silver, "DEBUG: Toggle crouching");
		}
		IG2IMovementInputInterface::Execute_ToggleCrouchAction(MovementComponent);
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("%s does not implemented movement interface"),
			*MovementComponent->GetName());
	}
}
#endif

void AG2IPlayerController::SelectNextCharacter(const FInputActionValue& Value)
{
    if (AG2IPlayerState *CurrentPlayerState = GetPlayerState<AG2IPlayerState>())
    {
        if (CurrentPlayerState->SelectNextCharacter())
        {
        	OnSwitchBetweenCharacterDelegate.Broadcast();
        }
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
		if (APawn* CurrentPawn = GetPawn())
		{
			CurrentPawn->bUseControllerRotationYaw = true;
		}
	}
}

void AG2IPlayerController::StopAiming(const FInputActionValue& Value)
{
	if (AimingComponent && AimingComponent->Implements<UG2IAimingInterface>())
	{
		IG2IAimingInterface::Execute_StopAimingAction(AimingComponent);
		if (APawn* CurrentPawn = GetPawn())
		{
			CurrentPawn->bUseControllerRotationYaw = false;
		}
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
	if (GlovePunchComponent)
	{
		IG2IGlovePunchInterface::Execute_GlovePunchActivation(GlovePunchComponent);
	}
}


#if WITH_EDITOR
void AG2IPlayerController::SaveGameplay(const FInputActionValue& Value)
{
	if (!bIsDebugAction)
	{
		return;
	}

	if (const auto* GameInstance = GetGameInstance())
	{

		if (UG2ISavingGameplayManager* SavingGameplayManager = GameInstance->GetSubsystem<UG2ISavingGameplayManager>())
		{
			SavingGameplayManager->SaveAllDataAndGameplay(true);

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Silver, "DEBUG: Saved gameplay data.");
			}
		}
		else
		{
			UE_LOG(LogG2I, Warning, TEXT("Couldn't get SavingGameplayManager subsystem from GameInstance in %s."), *GetName());
			return;
		}
	}
}

void AG2IPlayerController::LoadGameplay(const FInputActionValue& Value)
{
	if (!bIsDebugAction)
	{
		return;
	}

	if (const auto* GameInstance = GetGameInstance())
	{
		if (UG2ISavingGameplayManager* SavingGameplayManager = GameInstance->GetSubsystem<UG2ISavingGameplayManager>())
		{
			SavingGameplayManager->LoadGameplay(false);
			SavingGameplayManager->LoadAllData();
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Silver, "DEBUG: Gameplay data loaded.");
			}
		}
		else
		{
			UE_LOG(LogG2I, Warning, TEXT("Couldn't get SavingGameplayManager subsystem from GameInstance in %s."), *GetName());
			return;
		}
	}
}
#endif

void AG2IPlayerController::RotateCameraTo(const float Yaw, const float Pitch)
{
	for (UActorComponent* Component : ThirdPersonCameraComponents)
	{
		if (Component->Implements<UG2IThirdPersonCameraInputInterface>())
		{
			IG2IThirdPersonCameraInputInterface::Execute_RotateToAction(Component, Yaw, Pitch);
		}
	}
}

