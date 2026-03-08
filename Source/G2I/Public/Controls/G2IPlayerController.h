#pragma once

#include "CoreMinimal.h"
#include "EnhancedActionKeyMapping.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "Interfaces/SavingSystem/G2ISaveGameplayInterface.h"
#include "Interfaces/SavingSystem/G2ISavableInterface.h"
#include "G2IPlayerController.generated.h"

class UG2IUIManager;
class UG2ICameraDefaultsParameters;
class UCameraComponent;
struct FInputActionInstance;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnPossessPawnDelegate, APawn *, Pawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPossessPawnDelegate, APawn *, Pawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FToggleFollowAIBehindPlayerDelegate, bool, Value);

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class G2I_API AG2IPlayerController : public APlayerController, public IG2ISavableInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FUnPossessPawnDelegate OnUnPossessPawnDelegate;

	UPROPERTY(BlueprintAssignable)
	FPossessPawnDelegate OnPossessPawnDelegate;

	UPROPERTY(BlueprintAssignable)
	FToggleFollowAIBehindPlayerDelegate OnToggleFollowAIBehindPlayerDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	bool bIsFollowingAIBehindPlayer = true;
	
private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UG2ICameraDefaultsParameters> CameraDefaultsParameters;

	UPROPERTY()
	TObjectPtr<UG2IUIManager> UIManager;
	
public:

	AG2IPlayerController();

	virtual void OnPossess(APawn *NewPawn) override;

	virtual void OnUnPossess() override;

	virtual void SetViewTargetWithBlend(AActor* NewViewTarget, float BlendTime = 0,
		EViewTargetBlendFunction BlendFunc = VTBlend_Linear, float BlendExp = 0, bool bLockOutgoing = false) override;

	virtual bool SetPause(bool bPause, FCanUnpause CanUnpauseDelegate = FCanUnpause()) override;

public:

	void SetRotationTowardsCamera(const UCameraComponent& Camera);

	TObjectPtr<UG2ICameraDefaultsParameters> GetCameraDefaultsParameters();
	
	void QuitGame();

	FName GetKeyName(UInputAction *InputAction);

	TMap<TObjectPtr<UInputAction>, FName>& GetActionToTagMap();

public:
	/* Saving system */

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer GameplayTags;

	void SaveData_Implementation(UG2IGameplaySaveGame* SaveGameRef);

	void LoadData_Implementation(const UG2IGameplaySaveGame* SaveGameRef);
protected:

	/** Setup Input */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<TObjectPtr<UInputMappingContext>> InputMappingContexts;

	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<FEnhancedActionKeyMapping> InputKeyMappings;
	
	virtual void SetupInputComponent() override;

	void SetupCharacterActorComponents();

	void SetupCamera();

	/** Actions */
	/** Select camera actions */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UActorComponent> CameraControllersComponent;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SwitchCameraBehaviorAction;
	
	void SwitchCameraBehavior();

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

	/*UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ToggleCrouchAction;*/

	void ToggleCrouch(const FInputActionValue& Value);

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

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UActorComponent> SteamMovementComponent;

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
	TObjectPtr<UInputAction> FlightDownAction;
	

	void FlyUp(const FInputActionValue& Value);
	void FlyDown(const FInputActionValue& Value);
	
	void Fly(int Direction);

	void Shoot(const FInputActionValue& Value);
	
	/** Enable/disable AI characters to follow the player */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ToggleFollowAIBehindPlayerAction;

	void ToggleFollowAIBehindPlayer(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> PauseAction;

	void CallPause(const FInputActionValue& Value);
	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* GlovePunchAction;

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
