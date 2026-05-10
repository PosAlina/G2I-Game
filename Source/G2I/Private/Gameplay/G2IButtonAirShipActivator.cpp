#include "Gameplay/G2IButtonAirShipActivator.h"
#include "G2IMovingByGearObjectInterface.h"
#include "G2IWorldHintKeyWidgetComponent.h"
#include "LaunchingIndication/G2ILauncherComponent.h"
#include "G2I.h"
#include "Sound/G2ISoundComponent.h"

AG2IButtonAirShipActivator::AG2IButtonAirShipActivator()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AG2IButtonAirShipActivator::Interact_Implementation(const ACharacter* Interactor) {
	if (!ensure(LauncherComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*UG2ILauncherComponent::StaticClass()->GetName());
	}
	else if (bLockOnInteraction)
	{
		LauncherComp->SetIsLaunched(true);
	}

	UG2ISoundComponent::PlaySoundSafe(SoundComp, ActivationSoundId);

	bIsPushing = true;
	CurrentPushTime = 0.0f; 
	SetActorTickEnabled(true);
}

void AG2IButtonAirShipActivator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsPushing) 
	{ 
		UE_LOG(LogG2I, Warning, TEXT("%s: Trying to push while stop pushing"), *GetActorNameOrLabel());
		return; 
	}

	CurrentPushTime += DeltaTime;

	if (CurrentPushTime >= PushDuration)
	{
		bIsPushing = false;
		SetActorTickEnabled(false);
		UG2ISoundComponent::StopSoundSafe(SoundComp, ActivationSoundId);
		return;
	}

	float CurrentForce = PushForce * DeltaTime;

	for (AActor* Actor : ActorsToActivate)
	{
		if (!ensure(Actor))
		{
			UE_LOG(LogG2I, Warning, TEXT("%s: Actor is null while pushing"), *GetActorNameOrLabel());
			continue;
		}

		if (Actor->Implements<UG2IMovingByGearObjectInterface>())
		{
			IG2IMovingByGearObjectInterface::Execute_OnPushing(Actor, CurrentForce);
		}
	}
}

bool AG2IButtonAirShipActivator::CanInteract_Implementation(const ACharacter* Interactor)
{
	if (!ensure(LauncherComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*UG2ILauncherComponent::StaticClass()->GetName());
		return !bIsPushing;
	}
	if (LauncherComp->IsLocked_Implementation())
	{
		return false;
	}

	return !bIsPushing;
}