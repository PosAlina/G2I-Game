#include "LaunchingIndication/G2ILaunchingActorsIndicator.h"
#include "G2I.h"
#include "LaunchingIndication/G2ILauncherComponent.h"

void AG2ILaunchingActorsIndicator::BeginPlay()
{
	Super::BeginPlay();
	
	SetupTriggeringActorsInfo();
	BindDelegates();
	SetStartState();
}

void AG2ILaunchingActorsIndicator::SetupTriggeringActorsInfo()
{
	for (const auto& [StateName, StateInfo] : BaseStatesInfo)
	{
		SetupLaunchingActorsInfo(StateName, true,StateInfo.RequiredLaunchedActors);
		SetupLaunchingActorsInfo(StateName, false,StateInfo.RequiredStrictlyUnLaunchedActors);
		SetupActorsLockingInfo(StateInfo.LockedActors);
		SetupActorsLockingInfo(StateInfo.UnLockedActors);
	}
}

void AG2ILaunchingActorsIndicator::SetupLaunchingActorsInfo(
	const FName& StateName, const bool bIsLaunched, TSet<TObjectPtr<AActor>> ChangedActors)
{
	if (!ensure(!ChangedActors.Remove(nullptr)))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Set of required actor for %s state has null pointer"),
			*GetActorNameOrLabel(), *StateName.ToString());
	}
	for (AActor *Actor : ChangedActors)
	{
		FG2ITriggeringByIndicatorActorInfo& ActorInfo = SetupActorInfo(Actor);
		TSet<FName>& TargetStates = bIsLaunched ? ActorInfo.LaunchForStates : ActorInfo.UnLaunchForStates;
		TSet<FName>& ExcludingStates = bIsLaunched ? ActorInfo.UnLaunchForStates : ActorInfo.LaunchForStates;
		if (!ensure(!ExcludingStates.Contains(StateName)))
		{
			const FString ExcludingStatesSetName = bIsLaunched ? "Un launched set" : "Launched set";
			UE_LOG(LogG2I, Warning, TEXT(
				"%s: The actor %s is required to both launch and un launch for %s state. "
				"There is actor only in %s"), *GetActorNameOrLabel(), *Actor->GetActorNameOrLabel(),
				*StateName.ToString(), *ExcludingStatesSetName);
			continue;
		}
		TargetStates.Add(StateName);
	}
}

void AG2ILaunchingActorsIndicator::SetupActorsLockingInfo(TSet<TObjectPtr<AActor>> LockingActors)
{
	if (!ensure(!LockingActors.Remove(nullptr)))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Set of locking actor has null pointer"),
			*GetActorNameOrLabel());
	}
	for (AActor *Actor : LockingActors)
	{
		SetupActorInfo(Actor);
	}
}

FG2ITriggeringByIndicatorActorInfo& AG2ILaunchingActorsIndicator::SetupActorInfo(AActor* Actor)
{
	FG2ITriggeringByIndicatorActorInfo& ActorInfo = TriggeringActorsInfo.FindOrAdd(Actor);

	if (!ensure(Actor))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Attempt to setup actor info for null actor"), *GetActorNameOrLabel());
		return ActorInfo;
	}
	ActorInfo.LauncherComponent = Actor->FindComponentByClass<UG2ILauncherComponent>();
	if (!ensure(ActorInfo.LauncherComponent))
	{
		UE_LOG(LogG2I, Warning, TEXT(
			"%s: Actor %s doesn't have %s"), *GetActorNameOrLabel(),
			*Actor->GetActorNameOrLabel(), *UG2ILauncherComponent::StaticClass()->GetName());
		return ActorInfo;
	}

	ActorInfo.bIsLaunched = ActorInfo.LauncherComponent->IsLaunched();
	ActorInfo.bIsLocked = ActorInfo.LauncherComponent->IsLocked_Implementation();
	
	return ActorInfo;
}


void AG2ILaunchingActorsIndicator::BindDelegates()
{
	for (const auto &[_, ActorInfo] : TriggeringActorsInfo)
	{
		UG2ILauncherComponent *LauncherComponent = ActorInfo.LauncherComponent;
		if (!ensure(LauncherComponent))
		{
			UE_LOG(LogG2I, Warning, TEXT(
			"%s: Actor in set TriggeringActorsInfo doesn't have %s"), *GetActorNameOrLabel(),
			*UG2ILauncherComponent::StaticClass()->GetName());
			continue;
		}

		LauncherComponent->GetOnLaunchDelegate().AddDynamic(this, &ThisClass::ReactOnChangeLaunchingActorState);
		LauncherComponent->GetOnLockedDelegate().AddDynamic(this, &ThisClass::ReactOnChangeLockingActorState);
	}
}

void AG2ILaunchingActorsIndicator::SetStartState()
{
	if (SetCurrentStateWithDefaultState())
	{
		return;
	}

	TSet<FName> StatesNames;
	BaseStatesInfo.GetKeys(StatesNames);
	if (!ensure(CheckPotentialNewStatesAndSet(StatesNames)))
	{
		G2I::DebugWarningMessage(GetActorNameOrLabel() + "couldn't find any correct state");
	}
}

bool AG2ILaunchingActorsIndicator::SetCurrentStateWithDefaultState()
{
	if (DefaultState == NAME_None)
	{
		return false;
	}
	if (!ensure(BaseStatesInfo.Contains(DefaultState)))
	{
		G2I::DebugWarningMessage(GetActorNameOrLabel() + "couldn't find state with name " + DefaultState.ToString());
		return false;
	}
	
	if (CheckState(DefaultState))
	{
		SetState(DefaultState);
		return true;
	}
	G2I::DebugWarningMessage(GetActorNameOrLabel() + "couldn't set state with name " + DefaultState.ToString());
	return false;
}

bool AG2ILaunchingActorsIndicator::CheckPotentialNewStatesAndSet(TSet<FName>& States)
{
	for (const FName& StateName : States)
	{
		if (CheckState(StateName))
		{
			SetState(StateName);
			return true;
		}
	}
	return false;
}

bool AG2ILaunchingActorsIndicator::CheckState(const FName& StateName)
{
	const FG2ILaunchingActorsIndicatorState* StateInfo = BaseStatesInfo.Find(StateName);
	if (!StateInfo)
	{
		G2I::DebugWarningMessage(GetActorNameOrLabel() + "couldn't find state with name " + StateName.ToString());
	}

	for (AActor *Actor : StateInfo->RequiredLaunchedActors)
	{
		const FG2ITriggeringByIndicatorActorInfo *ActorInfo= TriggeringActorsInfo.Find(Actor);
		if (!ActorInfo->bIsLaunched)
		{
			return false;
		}
	}
	for (AActor *Actor : StateInfo->RequiredStrictlyUnLaunchedActors)
	{
		const FG2ITriggeringByIndicatorActorInfo *ActorInfo= TriggeringActorsInfo.Find(Actor);
		if (ActorInfo->bIsLaunched)
		{
			return false;
		}
	}
	
	return true;
}


void AG2ILaunchingActorsIndicator::SetState(const FName& NewState)
{
	CurrentState = NewState;
	G2I::DebugLogMessage(GetActorNameOrLabel() + " set state " + DefaultState.ToString());
	UpdateState();
}

void AG2ILaunchingActorsIndicator::UpdateState_Implementation()
{
	LockOrUnlockActors(BaseStatesInfo[CurrentState].UnLockedActors, false);
	LockOrUnlockActors(BaseStatesInfo[CurrentState].LockedActors, true);
}

void AG2ILaunchingActorsIndicator::ReactOnChangeLaunchingActorState(UG2ILauncherComponent* LauncherComponent,
	AActor* ComponentOwner, const bool bIsLaunched)
{
	if (FG2ITriggeringByIndicatorActorInfo *ActorInfo = TriggeringActorsInfo.Find(ComponentOwner))
	{
		if (ActorInfo->bIsLaunched == bIsLaunched)
		{
			return;
		}

		ActorInfo->bIsLaunched = bIsLaunched;
		if (CheckState(CurrentState))
		{
			return;
		}
		if (CheckPotentialNewStatesAndSet(ActorInfo->LaunchForStates))
		{
			return;
		}
		if (CheckPotentialNewStatesAndSet(ActorInfo->UnLaunchForStates))
		{
			return;
		}
	}
}

void AG2ILaunchingActorsIndicator::ReactOnChangeLockingActorState(UG2ILauncherComponent* LauncherComponent,
	AActor* ComponentOwner, const bool bIsLocked)
{
	if (FG2ITriggeringByIndicatorActorInfo *ActorInfo = TriggeringActorsInfo.Find(ComponentOwner))
	{
		ActorInfo->bIsLocked = bIsLocked;
	}
}

void AG2ILaunchingActorsIndicator::LockOrUnlockActors(TSet<TObjectPtr<AActor>> Actors, const bool bIsLocked)
{
	if (bIsLocked && !bIsEnableLock)
	{
		return;
	}
	for (AActor *Actor : Actors)
	{
		FG2ITriggeringByIndicatorActorInfo *ActorInfo= TriggeringActorsInfo.Find(Actor);
		if (ActorInfo->bIsLocked == bIsLocked)
		{
			continue;
		}

		ActorInfo->bIsLocked = bIsLocked;
		UG2ILauncherComponent* LauncherComponent= ActorInfo->LauncherComponent;
		if (!ensure(LauncherComponent))
		{
			UE_LOG(LogG2I, Warning, TEXT(
			"%s: Actor in set TriggeringActorsInfo doesn't have %s"), *GetActorNameOrLabel(),
			*UG2ILauncherComponent::StaticClass()->GetName());
			continue;
		}
		LauncherComponent->SetIsLocked_Implementation(bIsLocked);
	}
}

FName AG2ILaunchingActorsIndicator::GetCurrentState() const
{
	return CurrentState;
}

FName AG2ILaunchingActorsIndicator::GetDefaultState() const
{
	return DefaultState;
}
