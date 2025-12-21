#include "Components/SteamGlove/G2IReloadingComponent.h"
#include "G2I.h"
#include "G2ISteamMovementComponent.h"
#include "GameFramework/Character.h"

void UG2IReloadingComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupDefaults();
	BindDelegates();
}

void UG2IReloadingComponent::Reload()
{
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}
	
	OnStartReloadingDelegate.Broadcast();
	ShowReloadingOnView();
	World->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::FinishReloading, ReloadTime,
		false);
}

void UG2IReloadingComponent::FinishReloading()
{
	OnFinishReloadingDelegate.Broadcast();
}

void UG2IReloadingComponent::ShowReloadingOnView()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, ReloadTime, FColor::Red, TEXT("Glove is reloading"));
	}
}

void UG2IReloadingComponent::SetupDefaults()
{
	World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}
}

void UG2IReloadingComponent::BindDelegates()
{
	AActor *OwnerActor = GetOwner();
	if (!ensure(OwnerActor))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
		return;
	}

	const ACharacter *Owner = Cast<ACharacter>(OwnerActor);
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("%s isn't character in %s"), *OwnerActor->GetActorNameOrLabel(),
			*GetName());
		return;
	}

	if (UG2ISteamMovementComponent *SteamMovementComponent = Owner->FindComponentByClass<UG2ISteamMovementComponent>())
	{
		SteamMovementComponent->OnStartSteamJumpDelegate.AddDynamic(this, &ThisClass::Reload);
	}
}
