#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "G2ILaunchingActorsIndicator.generated.h"

class UG2ILauncherComponent;

USTRUCT(BlueprintType)
struct FG2ILaunchingActorsIndicatorState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = States)
	TSet<TObjectPtr<AActor>> RequiredLaunchedActors;

	UPROPERTY(EditAnywhere, Category = States)
	TSet<TObjectPtr<AActor>> RequiredStrictlyUnLaunchedActors;

	UPROPERTY(EditAnywhere, Category = States)
	TSet<TObjectPtr<AActor>> LockedActors;

	UPROPERTY(EditAnywhere,Category = States)
	TSet<TObjectPtr<AActor>> UnLockedActors;
};

/**
 * This struct is for internal use in the indicator:
 * set of states which the actor is launched
 * and set of states which the actor is strictly un launched
 */
USTRUCT(BlueprintType)
struct FG2ITriggeringByIndicatorActorInfo
{
	GENERATED_BODY()

	TSet<FName> LaunchForStates;

	TSet<FName> UnLaunchForStates;

	UPROPERTY()
	TObjectPtr<UG2ILauncherComponent> LauncherComponent;
	
	bool bIsLaunched = false;

	bool bIsLocked = false;
};

/**
 * The indicator that changed states
 * depending on which monitored actors are launching and which are not.
 * If several states can occur at the same time, the first one is chosen
 * (Please, don't set such state's description. Ensure will tell you if it's wrong)
 */
UCLASS()
class G2I_API AG2ILaunchingActorsIndicator : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = States)
	TMap<FName, FG2ILaunchingActorsIndicatorState> BaseStatesInfo;

	UPROPERTY(EditAnywhere, Category = States, meta=(ToolTip = "Indicator can lock and unlock actors"))
	bool bIsEnableLock = true;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true,
		ToolTip = "Optional: You can set default state"))
	FName DefaultState = NAME_None;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FName CurrentState = NAME_None;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TMap<TObjectPtr<AActor>, FG2ITriggeringByIndicatorActorInfo> TriggeringActorsInfo;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void ReactOnChangeLaunchingActorState(UG2ILauncherComponent *LauncherComponent, AActor *ComponentOwner,
		bool bIsLaunched);

	UFUNCTION()
	void ReactOnChangeLockingActorState(UG2ILauncherComponent *LauncherComponent, AActor *ComponentOwner,
		bool bIsLocked);
	
	UFUNCTION(BlueprintNativeEvent)
	void UpdateState();

	FName GetCurrentState() const;
	FName GetDefaultState() const;

private:
	
	void SetupTriggeringActorsInfo();
	void SetupLaunchingActorsInfo(const FName& StateName, bool bIsLaunched, TSet<TObjectPtr<AActor>> ChangedActors);
	void SetupActorsLockingInfo(TSet<TObjectPtr<AActor>> LockingActors);
	FG2ITriggeringByIndicatorActorInfo& SetupActorInfo(AActor *Actor);
	
	void BindDelegates();

	bool CheckState(const FName& StateName);
	bool CheckPotentialNewStatesAndSet(TSet<FName>& States);

	void LockOrUnlockActors(TSet<TObjectPtr<AActor>> Actors, bool bIsLocked);
	void SetStartState();
	bool SetCurrentStateWithDefaultState();
	
	void SetState(const FName& NewState);

};