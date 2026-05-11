#pragma once

#include "CoreMinimal.h"
#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputSubsystems.h"
#include "G2IAimingComponent.h"
#include "GameFramework/PlayerController.h"
#include "G2IPlayerController.generated.h"

class UG2IUIManager;
class UG2ICameraDefaultsParameters;
class UCameraComponent;
struct FInputActionInstance;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

UENUM(BlueprintType)
enum class EG2IInputMappingContextType : uint8
{
	Constant UMETA(DisplayName = "Gamemechanics doesn't affect"),
	Changeable UMETA(DisplayName = "Can be overriden")
};

USTRUCT(BlueprintType)
struct FG2IInputMappingContexts
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<TObjectPtr<UInputMappingContext>> Contexts;

	void AddAllContextsToMapping(UEnhancedInputLocalPlayerSubsystem& Subsystem) const;
	void RemoveAllContextsToMapping(UEnhancedInputLocalPlayerSubsystem& Subsystem) const;
};

USTRUCT(BlueprintType)
struct FG2IInputMappingContextsByType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TMap<EG2IInputMappingContextType, FG2IInputMappingContexts> ContextByType;
};

USTRUCT(BlueprintType)
struct FG2IInputKeyMapping
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FEnhancedActionKeyMapping> Mappings;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnPossessPawnDelegate, APawn *, Pawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPossessPawnDelegate, APawn *, Pawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FToggleFollowAIBehindPlayerDelegate, bool, Value);
DECLARE_MULTICAST_DELEGATE(FSwitchBetweenCharacterDelegate);
DECLARE_MULTICAST_DELEGATE(FFlyUpDelegate);

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class G2I_API AG2IPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	
	FSwitchBetweenCharacterDelegate OnSwitchBetweenCharacterDelegate;
	
	FFlyUpDelegate OnFlyUpDelegate;

	UPROPERTY(BlueprintAssignable)
	FUnPossessPawnDelegate OnUnPossessPawnDelegate;

	UPROPERTY(BlueprintAssignable)
	FPossessPawnDelegate OnPossessPawnDelegate;

	UPROPERTY(BlueprintAssignable)
	FToggleFollowAIBehindPlayerDelegate OnToggleFollowAIBehindPlayerDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	bool bIsFollowingAIBehindPlayer = true;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveSliderAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SliderExitAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SliderPushAction;
	
private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UG2ICameraDefaultsParameters> CameraDefaultsParameters;

	UPROPERTY()
	TObjectPtr<UG2IUIManager> UIManager;
	
public:

	AG2IPlayerController();

	virtual void SetViewTargetWithBlend(AActor* NewViewTarget, float BlendTime = 0,
		EViewTargetBlendFunction BlendFunc = VTBlend_Linear, float BlendExp = 0, bool bLockOutgoing = false) override;

public:

	void SetRotationTowardsCamera(const UCameraComponent& Camera);

	TObjectPtr<UG2ICameraDefaultsParameters> GetCameraDefaultsParameters();
	
	void QuitGame();

	FName GetKeyName(UInputAction *InputAction, const TSubclassOf<APawn>& PawnClass);

	TMap<TObjectPtr<UInputAction>, FName>& GetActionToTagMap();

	void OverrideInputMappingContext(const TArray<TObjectPtr<UInputMappingContext>>& ContextsForOverride);
	void StopOverrideInputMappingContext();
	void OverrideInputMappingContextToSlider();

	UEnhancedInputLocalPlayerSubsystem *GetSubsystem() const;
	TSubclassOf<APawn> GetPawnClass(const APawn* NewPawn) const;
	TSubclassOf<APawn> GetCurrentPawnClass() const;
	UG2IAimingComponent *GetAimingComponent() const;
	TMap<TSubclassOf<APawn>, FG2IInputKeyMapping>& GetInputKeyMapping();
	UInputAction *GetMoveAction();
	UInputAction *GetMoveSliderAction();
	
	bool IsCurrentPawnClass(const TSubclassOf<APawn>& PawnClass) const;
	
	void RotateCameraTo(float Yaw, float Pitch);

	bool bIsNeedToSkipFirstJump = false;

protected:
	
	virtual void OnPossess(APawn *NewPawn) override;
	virtual void OnUnPossess() override;

	void SetupDefaults();
	void SetupKeyMapping();
	void SetupCommonInput();
	void BindEnhancedDelegates();
	
	void SetupInputForPawn(const APawn *NewPawn);
	void SetupInputIfPawnClassIsCurrent(const TSubclassOf<APawn>& PawnClass);
	void SetupInputOverridenForPawn(
		const FG2IInputMappingContexts& ContextsInfoForOverriden, UEnhancedInputLocalPlayerSubsystem& Subsystem);
	void SetupInputWithoutOverridenForPawn(
		const TSubclassOf<APawn>& PawnClass, UEnhancedInputLocalPlayerSubsystem& Subsystem);
	
	void RemovedInputForPawn(const APawn *NewPawn);
	void RemovedInputIfPawnClassIsCurrent(const TSubclassOf<APawn>& PawnClass);
	void RemovedInputOverridenForPawn(
		const FG2IInputMappingContexts& ContextsInfoForOverriden, UEnhancedInputLocalPlayerSubsystem& Subsystem);
	void RemovedInputWithoutOverridenForPawn(
		const TSubclassOf<APawn>& PawnClass, UEnhancedInputLocalPlayerSubsystem& Subsystem);

	/** Setup Input */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TMap<TSubclassOf<APawn>, FG2IInputMappingContexts> InputMappingContextsByPawn;
	
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TMap<EG2IInputMappingContextType, FG2IInputMappingContexts> CommonInputMappingContexts;

	UPROPERTY()
	TMap<TSubclassOf<APawn>, FG2IInputMappingContexts> InputMappingContextsForOverridingByPawn;
	
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TObjectPtr<UInputMappingContext> SliderInputMappingContext;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category ="Input|Debug|Input Mappings")
	TMap<TSubclassOf<APawn>, FG2IInputMappingContexts> DebugInputMappingContextsByPawn;

	UPROPERTY(EditAnywhere, Category ="Input|Debug|Input Mappings")
	TMap<EG2IInputMappingContextType, FG2IInputMappingContexts> DebugCommonInputMappingContexts;
#endif

	UPROPERTY()
	TMap<TSubclassOf<APawn>, FG2IInputKeyMapping> InputKeyMappings;
	
	virtual void SetupInputComponent() override;

	void SetupCharacterActorComponents();

	void SetupCamera() const;

	/** Actions */
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category="Input|Debug")
	TObjectPtr<UInputAction> ToggleDebugAction;

	bool bIsDebugAction = false;

	void ToggleDebugMode();
	
	UPROPERTY(EditAnywhere, Category="Input|Debug")
	TObjectPtr<UInputAction> SwitchCameraBehaviorAction;
	
	void SwitchCameraBehavior();

	UPROPERTY(EditAnywhere, Category = "Input|Debug")
	TObjectPtr<UInputAction> ToggleCrouchAction;

	void ToggleCrouch();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UActorComponent> SteamMovementComponent;

	UPROPERTY(EditAnywhere, Category="Input|Debug")
	TObjectPtr<UInputAction> DebugPauseAction;
#endif

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UActorComponent> CameraControllersComponent;
	
	UPROPERTY(VisibleAnywhere)
	TSet<TObjectPtr<UActorComponent>> ThirdPersonCameraComponents;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> LookAction;

	void Look(const FInputActionValue& Value);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UActorComponent> MovementComponent;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;
	
	void Move(const FInputActionValue& Value);
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> JumpAction;

	void Jump(const FInputActionValue& Value);
	
	void StopJumping(const FInputActionValue& Value);

	/** Select character actions */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SelectNextCharacterAction;

	void SelectNextCharacter(const FInputActionValue& Value);

	/** Interact Actions */
	UPROPERTY(EditAnywhere, Category = "Input")
	TSet<TObjectPtr<UInputAction>> InteractActions;

	UPROPERTY(VisibleAnywhere)
	TSet<TObjectPtr<UActorComponent>> InteractionComponents;

	void Interact(const FInputActionInstance& Instance);

	UPROPERTY(EditAnywhere, Category = "Data")
	TMap<TObjectPtr<UInputAction>, FName> ActionToTagMap;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> TakeAimAction;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UActorComponent> AimingComponent;

	void StartAiming(const FInputActionValue& Value);

	void StopAiming(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ShootAction;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UActorComponent> SteamShotComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UActorComponent> FlightComponent;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> FlightUpAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> FlightDownAction;
	

	void FlyUp(const FInputActionValue& Value);
	void FlyDown(const FInputActionValue& Value);
	
	void Fly(int Direction) const;

	void StopFlight(const FInputActionValue& Value);

	void Shoot(const FInputActionValue& Value);
	
	/** Enable/disable AI characters to follow the player */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ToggleFollowAIBehindPlayerAction;

	void ToggleFollowAIBehindPlayer(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> PauseAction;

	void CallPause(const FInputActionValue& Value);
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> GlovePunchAction;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UActorComponent> GlovePunchComponent;
	
	void GlovePunchActivation(const FInputActionInstance& Instance);

	/** Debug keys for testing saving system */

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Input|Debug")
	TObjectPtr<UInputAction> SaveAction;

	UPROPERTY(EditAnywhere, Category = "Input|Debug")
	TObjectPtr<UInputAction> LoadAction;
#endif

#if WITH_EDITOR
	void SaveGameplay(const FInputActionValue& Value);

	void LoadGameplay(const FInputActionValue& Value);
#endif
};