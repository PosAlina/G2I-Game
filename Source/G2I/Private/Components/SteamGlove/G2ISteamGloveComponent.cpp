#include "Components/SteamGlove/G2ISteamGloveComponent.h"
#include "G2ISteamMovementComponent.h"
#include "Components/SteamGlove/G2IReloadingComponent.h"

UG2ISteamGloveComponent::UG2ISteamGloveComponent()
{
	ReloadingComp = CreateDefaultSubobject<UG2IReloadingComponent>(FName("ReloadingComp"));
	SteamMovementComp = CreateDefaultSubobject<UG2ISteamMovementComponent>(FName("SteamMovementComp"));
}