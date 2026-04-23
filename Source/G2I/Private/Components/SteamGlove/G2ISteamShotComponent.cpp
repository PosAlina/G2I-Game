#include "G2ISteamShotComponent.h"
#include "G2I.h"
#include "G2ITraceableObectInterface.h"
#include "G2IAimTypeEnum.h"
#include "Sound/G2ISoundComponent.h"

void UG2ISteamShotComponent::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
	}

	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		UG2ISoundComponent* FoundSoundComp = MyOwner->FindComponentByClass<UG2ISoundComponent>();

		if (FoundSoundComp)
		{
			SoundComponent = FoundSoundComp;
		}
	}

	if (SoundComponent && SoundComponent->SetupSounds.Contains(TEXT("SteamShotSound")))
	{
		SteamSoundId = SoundComponent->AddSound(SoundComponent->SetupSounds[TEXT("SteamShotSound")]);
		if (SteamSoundId == -1)
		{
			UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: SteamSoundId (ID == -1)"), *GetName(), *FString(__FUNCTION__));
		}
	}
}

void UG2ISteamShotComponent::ShootAction_Implementation(const FG2IHitInfo TargetHitInfo)
{
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}

	const FVector StartShootLocation = GetComponentLocation();
	DrawDebugLine(World, StartShootLocation, TargetHitInfo.HitResult.Location, FColor::Red, false, 1.f);

	if (SoundComponent)
	{
		if (!SoundComponent->IsSoundPlaying(SteamSoundId)) {
			SoundComponent->PlaySound(SteamSoundId);
		}
	}

	if (TargetHitInfo.HitSuccess)
	{
		if (!TargetHitInfo.HitResult.GetActor()) {
			OnAimTypeAfterHitChangedDelegate.Broadcast(EG2IAimType::MissedHitAim);
			UE_LOG(LogG2I, Warning, TEXT("Shooting actor was deleted"));
			return;
		}
		if (TargetHitInfo.HitResult.GetActor()->Implements<UG2ITraceableObectInterface>()) {
			OnAimTypeAfterHitChangedDelegate.Broadcast(EG2IAimType::SuccessfulHitAim);
			IG2ITraceableObectInterface::Execute_OnShoot(TargetHitInfo.HitResult.GetActor(), TargetHitInfo.HitResult, GetOwner());
			return;
		}
	}
	OnAimTypeAfterHitChangedDelegate.Broadcast(EG2IAimType::MissedHitAim);
}

void UG2ISteamShotComponent::ActivateForAim_Implementation()
{
	OnComponentActivateDelegate.Broadcast(ShotDistance);
}

/**
 * This function describes
 * what behavior the aim is expected
 * when aiming at different types of actors
 */
EG2IAimType UG2ISteamShotComponent::GetAimTypeByActor_Implementation(const AActor *TargetActor)
{
	if (TargetActor && TargetActor->Implements<UG2ITraceableObectInterface>())
	{
		return EG2IAimType::TargetAim;
	}
	return EG2IAimType::DefaultAim;
}