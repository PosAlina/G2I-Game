#include "G2ISteamShotComponent.h"
#include "G2I.h"
#include "G2ITraceableObectInterface.h"
#include "G2IAimTypeEnum.h"

void UG2ISteamShotComponent::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
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
	if (TargetHitInfo.HitSuccess)
	{
		
		OnAimTypeAfterHitChangedDelegate.Broadcast(EG2IAimType::SuccessfulHitAim);
		DrawDebugSphere(World, TargetHitInfo.HitResult.ImpactPoint, 8.0f, 12, FColor::Red, false, 1.f);
		if (TargetHitInfo.HitResult.GetActor()->Implements<UG2ITraceableObectInterface>()) {
			IG2ITraceableObectInterface::Execute_OnShoot(TargetHitInfo.HitResult.GetActor(), TargetHitInfo.HitResult, GetOwner());
		}
	}
	else
	{
		OnAimTypeAfterHitChangedDelegate.Broadcast(EG2IAimType::MissedHitAim);
	}
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
	if (TargetActor)
	{
		return EG2IAimType::TargetAim;
	}
	else
	{
		return EG2IAimType::DefaultAim;
	}
}