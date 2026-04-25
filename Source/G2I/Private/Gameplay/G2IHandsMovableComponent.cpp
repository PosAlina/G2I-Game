#include "G2IHandsMovableComponent.h"
#include "G2I.h"

void UG2IHandsMovableComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Owner = GetOwner();
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
	}
	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World is null in %s"), *GetName());
		return;
	}
	PlayerController = Cast<APlayerController>(World->GetFirstPlayerController());
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(), *APlayerController::StaticClass()->GetName());
		return;
	}
}


bool UG2IHandsMovableComponent::CanMove() const
{
	return !BanCharacters.Contains(PlayerController->GetPawn());
}

void UG2IHandsMovableComponent::PrepareBeforePutDown()
{
	for (const auto [Component, bIsSimulatePhysics, CollisionObjectType,
		CollisionProfileName] : PrimitiveComponents)
	{
		Component->SetSimulatePhysics(bIsSimulatePhysics);
		Component->SetCollisionObjectType(CollisionObjectType);
		Component->SetCollisionProfileName(CollisionProfileName);
	}
}

void UG2IHandsMovableComponent::PrepareBeforeGrabPickedUp()
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
	}
	TArray<UPrimitiveComponent*> Components;
	Owner->GetComponents<UPrimitiveComponent>(Components);
	for (UPrimitiveComponent* Component : Components)
	{
		if (!Component)
		{
			continue;
		}
		RememberComponentInfo(*Component);
		PrepareOneComponentBeforeGrabPickedUp(*Component);
	}
}

void UG2IHandsMovableComponent::RememberComponentInfo(UPrimitiveComponent& Component)
{
	FG2IDefaultParametersOfPrimitiveComponent DefaultParameters;
	DefaultParameters.PrimitiveComponent = Component;
	DefaultParameters.bIsSimulatePhysics = Component.BodyInstance.bSimulatePhysics;
	DefaultParameters.CollisionObjectType = Component.GetCollisionObjectType();
	DefaultParameters.CollisionProfileName = Component.GetCollisionProfileName();
	PrimitiveComponents.Add(DefaultParameters);
}

void UG2IHandsMovableComponent::PrepareOneComponentBeforeGrabPickedUp(UPrimitiveComponent& Component)
{
	Component.SetSimulatePhysics(true);
	Component.SetCollisionObjectType(ECC_GameTraceChannel7);
	Component.SetCollisionProfileName("MovableActor");
}

void UG2IHandsMovableComponent:: PrepareBeforeAttachedPickedUp()
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
	}
	TArray<UPrimitiveComponent*> Components;
	Owner->GetComponents<UPrimitiveComponent>(Components);
	for (UPrimitiveComponent* Component : Components)
	{
		if (!Component)
		{
			continue;
		}
		RememberComponentInfo(*Component);
		PrepareOneComponentBeforeAttachedPickedUp(*Component);
	}
}

void UG2IHandsMovableComponent::PrepareOneComponentBeforeAttachedPickedUp(UPrimitiveComponent& Component)
{
	Component.SetSimulatePhysics(false);
	Component.SetCollisionObjectType(ECC_GameTraceChannel7);
	Component.SetCollisionProfileName("MovableActor");
}