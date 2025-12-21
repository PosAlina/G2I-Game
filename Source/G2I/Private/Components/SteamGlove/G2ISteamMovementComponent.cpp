#include "G2ISteamMovementComponent.h"
#include "G2I.h"
#include "G2ICharacterMovementComponent.h"
#include "G2IReloadingComponent.h"
#include "GameFramework/Character.h"

void UG2ISteamMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupDefaults();
	BindDelegates();
}

void UG2ISteamMovementComponent::SteamJumpAction_Implementation()
{
	if (!bCanSteamJump)
	{
		return;
	}
	
	if (SteamJumpCurrentCount++ < SteamJumpMaxCount)
	{
		OnStartSteamJumpDelegate.Broadcast();
		if (MovementComponent && MovementComponent->Implements<UG2IMovementInputInterface>())
		{
			IG2IMovementInputInterface::Execute_ImpulseJumpAction(MovementComponent, SteamJumpImpulseStrength);
		}
	}
}

void UG2ISteamMovementComponent::HandleLanded(const FHitResult& Hit)
{
	SteamJumpCurrentCount = 0;
}

void UG2ISteamMovementComponent::EnableSteamJump()
{
	bCanSteamJump = true;
}

void UG2ISteamMovementComponent::DisableSteamJump()
{
	bCanSteamJump = false;
}

void UG2ISteamMovementComponent::SetupDefaults()
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

	MovementComponent = Owner->GetCharacterMovement();
	if (!ensure(MovementComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("%s hasn't movement component in %s"), *Owner->GetActorNameOrLabel(),
			*GetName());
		return;
	}
	if (!ensure(MovementComponent->Implements<UG2IMovementInputInterface>()))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s does not implemented movement interface"),
			*MovementComponent->GetName());
	}
}

void UG2ISteamMovementComponent::BindDelegates()
{
	AActor *OwnerActor = GetOwner();
	if (!ensure(OwnerActor))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
		return;
	}
	
	ACharacter *Owner = Cast<ACharacter>(OwnerActor);
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("%s isn't character in %s"), *OwnerActor->GetActorNameOrLabel(),
			*GetName());
		return;
	}
	
	Owner->LandedDelegate.AddDynamic(this, &ThisClass::HandleLanded);

	if (UG2IReloadingComponent *ReloadingComponent = Owner->FindComponentByClass<UG2IReloadingComponent>())
	{
		ReloadingComponent->OnStartReloadingDelegate.AddDynamic(this, &ThisClass::DisableSteamJump);
		ReloadingComponent->OnFinishReloadingDelegate.AddDynamic(this, &ThisClass::EnableSteamJump);
	}
}
