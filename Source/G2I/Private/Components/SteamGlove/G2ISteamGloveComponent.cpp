#include "Components/SteamGlove/G2ISteamGloveComponent.h"
#include "G2ISteamMovementComponent.h"
#include "Components/SteamGlove/G2IReloadingComponent.h"
#include "Components/SteamGlove/G2IGlovePunchComponent.h"

UG2ISteamGloveComponent::UG2ISteamGloveComponent()
{
	ReloadingComp = CreateDefaultSubobject<UG2IReloadingComponent>(FName("ReloadingComp"));
	GlovePunchComp = CreateDefaultSubobject<UG2IGlovePunchComponent>(FName("GlovePunchComp"));
}