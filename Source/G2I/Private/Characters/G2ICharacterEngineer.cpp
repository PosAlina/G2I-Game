#include "G2ICharacterEngineer.h"
#include "Components/Camera/G2IThirdPersonCameraComponent.h"
#include "Components/G2ICharacterCollisionComponent.h"
#include "Components/G2ICharacterMovementComponent.h"
#include "Components/G2IInventoryComponent.h"
#include "Components/G2IInteractionComponent.h"
#include "Components/SteamGlove/G2ISteamGloveComponent.h"
#include "Components/Camera/G2ICameraControllerComponent.h"
#include "Components/Camera/G2IFixedCamerasComponent.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "G2I.h"

AG2ICharacterEngineer::AG2ICharacterEngineer(const FObjectInitializer& ObjectInitializer)
	: ACharacter(ObjectInitializer.SetDefaultSubobjectClass<UG2ICharacterMovementComponent>(
		CharacterMovementComponentName))
{
	CollisionComp = CreateDefaultSubobject<UG2ICharacterCollisionComponent>(TEXT("CollisionComp"));

	InteractionComp = CreateDefaultSubobject<UG2IInteractionComponent>(TEXT("InteractionComp"));
	InventoryComp = CreateDefaultSubobject<UG2IInventoryComponent>(TEXT("InventoryComp"));

	CameraControllerComp = CreateDefaultSubobject<UG2ICameraControllerComponent>(TEXT("CameraControllerComp"));
	ThirdPersonCameraComp = CreateDefaultSubobject<UG2IThirdPersonCameraComponent>(TEXT("ThirdPersonCameraComp"));
	FixedCamerasComp = CreateDefaultSubobject<UG2IFixedCamerasComponent>(TEXT("FixedCamerasComp"));

	ValveInteractionComp = CreateDefaultSubobject<UG2IValveInteractionComponent>(TEXT("ValveInteractionComp"));
	HoleInteractionComp = CreateDefaultSubobject<UG2IHoleInteractionComponent>(TEXT("HoleInteractionComp"));
	SteamGloveComp = CreateDefaultSubobject<UG2ISteamGloveComponent>(TEXT("SteamGloveComp"));

	if (!ensure(InventoryComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s hasn't inventory component %s"), *GetName(), *UG2IInventoryComponent::StaticClass()->GetName());
		return;
	}
	InventoryComp->PickupItemTag = TEXT("Engineer");
}


void AG2ICharacterEngineer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	OnPossessedDelegate.Broadcast(NewController);
}

void AG2ICharacterEngineer::UnPossessed()
{
	Super::UnPossessed();

	OnUnPossessedDelegate.Broadcast();
}

FPossessedDelegate& AG2ICharacterEngineer::GetPossessedDelegate()
{
	return OnPossessedDelegate;
}

FUnPossessedDelegate& AG2ICharacterEngineer::GetUnPossessedDelegate()
{
	return OnUnPossessedDelegate;
}