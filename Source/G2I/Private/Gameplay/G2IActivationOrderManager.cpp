#include "Gameplay/G2IActivationOrderManager.h"
#include "G2I.h"
#include "G2IInteractiveObjectInterface.h"
#include "G2IWorldHintKeyWidgetComponent.h"
#include "LaunchingIndication/G2ILauncherComponent.h"

AG2IActivationOrderManager::AG2IActivationOrderManager()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	if (RootSceneComponent)
		SetRootComponent(RootSceneComponent);

	LauncherComp = CreateDefaultSubobject<UG2ILauncherComponent>(TEXT("LauncherComp"));
	if (!ensure(LauncherComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*UG2ILauncherComponent::StaticClass()->GetName());
	}
}

void AG2IActivationOrderManager::OrderCompleted()
{
	OnActivationWithOrderEndedDelegate.Broadcast(this, true);
	if (!ensure(LauncherComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*UG2ILauncherComponent::StaticClass()->GetName());
	}
	else
	{
		LauncherComp->SetIsLaunched(true);
	}
	
	UnbindToAllDelegates();
	UE_LOG(LogG2I, Log, TEXT("Order completed successfully in %s"), *GetActorNameOrLabel());
}

void AG2IActivationOrderManager::OrderFailed()
{
	CurrentIndex = 0;
	for (const AActor* Actor : ActivatedActorsArray)
	{
		if (!ensure(Actor))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: In ActivatedActorsArray actor is null"), *GetActorNameOrLabel());
			continue;
		}
		if (auto* ActivationComponent = Actor->GetComponentByClass<UG2IActivationWithOrderComponent>())
		{
			ActivationComponent->Restored();
		}
		else
		{
			UE_LOG(LogG2I, Error, TEXT("Failed to get ActivationWithOrder component from %s actor in %s"),
				*Actor->GetActorNameOrLabel(), *GetActorNameOrLabel());
		}
	}
	ActivatedActorsArray.Empty();
	OnActivationWithOrderEndedDelegate.Broadcast(this, false);
	UE_LOG(LogG2I, Log, TEXT("Order failed in %s"), *GetActorNameOrLabel());
}

void AG2IActivationOrderManager::OrderCancelled()
{
	CurrentIndex = 0;
	for (const AActor* Actor : ActivatedActorsArray)
	{
		if (!ensure(Actor))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: In ActivatedActorsArray actor is null"), *GetActorNameOrLabel());
			continue;
		}
		if (auto* ActivationComponent = Actor->GetComponentByClass<UG2IActivationWithOrderComponent>())
		{
			ActivationComponent->Restored();
		}
		else
		{
			UE_LOG(LogG2I, Error, TEXT("Failed to get ActivationWithOrder component from %s actor in %s"),
				*Actor->GetActorNameOrLabel(), *GetActorNameOrLabel());
		}
	}
	ActivatedActorsArray.Empty();
	OnActivationWithOrderEndedDelegate.Broadcast(this, false);
	UE_LOG(LogG2I, Log, TEXT("Order cancelled in %s"), *GetActorNameOrLabel());
}

void AG2IActivationOrderManager::BeginPlay()
{
	Super::BeginPlay();
	
	BindToAllDelegates();
	SetupDefaults();
}

void AG2IActivationOrderManager::SetupDefaults()
{
	NumberOfActors = CorrectOrderOfActors.Num();
	ActivatedActorsArray.Reserve(NumberOfActors);
	
	if (!ensure(LauncherComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*UG2ILauncherComponent::StaticClass()->GetName());
		return;
	}
	for (AActor* Actor : CorrectOrderOfActors)
	{
		if (!ensure(Actor))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: In ActivatedActorsArray actor is null"), *GetActorNameOrLabel());
			continue;
		}
		if (Actor->Implements<UG2ILockingInterface>())
		{
			IG2ILockingInterface::Execute_SetIsLocked(Actor, LauncherComp->IsLocked_Implementation());
		}
	}
}

void AG2IActivationOrderManager::BindToAllDelegates()
{
	for (const AActor* Actor : CorrectOrderOfActors)
	{
		if (!ensure(Actor))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: In ActivatedActorsArray actor is null"), *GetActorNameOrLabel());
			continue;
		}
		if (auto* ActivationComponent = Actor->GetComponentByClass<UG2IActivationWithOrderComponent>())
		{
			ActivationComponent->OnActivatedDelegate.BindUObject(this, &AG2IActivationOrderManager::OnActorActivated);
		}
		else
		{
			UE_LOG(LogG2I, Error, TEXT("Failed to get ActivationWithOrder component from %s actor in %s"),
				*Actor->GetActorNameOrLabel(), *GetActorNameOrLabel());
		}
	}
	if (!ensure(LauncherComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*UG2ILauncherComponent::StaticClass()->GetName());
		return;
	}
	LauncherComp->GetOnLockedDelegate().AddDynamic(this, &ThisClass::LockedCorrectOrderActors);
}

void AG2IActivationOrderManager::UnbindToAllDelegates()
{
	for (const AActor* Actor : CorrectOrderOfActors)
	{
		if (!ensure(Actor))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: In ActivatedActorsArray actor is null"), *GetActorNameOrLabel());
			continue;
		}
		if (auto* ActivationComponent = Actor->GetComponentByClass<UG2IActivationWithOrderComponent>())
		{
			ActivationComponent->OnActivatedDelegate.Unbind();
		}
		else
		{
			UE_LOG(LogG2I, Error, TEXT("Failed to get ActivationWithOrder component from %s actor in %s"),
				*Actor->GetActorNameOrLabel(), *GetActorNameOrLabel());
		}
	}
}

void AG2IActivationOrderManager::OnActorActivated(AActor* ActivatedActor, bool bReactivation, UG2IActivationWithOrderComponent* ActivationComponent)
{
	ActivatedActorsArray.Add(ActivatedActor);
	if (ActivationComponent)
		ActivationComponent->Accepted(CurrentIndex);
	CurrentIndex++;
	CheckIfOrderCompleted();
}

void AG2IActivationOrderManager::CheckIfOrderCompleted()
{
	if (CurrentIndex < NumberOfActors)
		return;

	for (int32 i = 0; i < NumberOfActors; i++)
	{
		if (CorrectOrderOfActors[i] != ActivatedActorsArray[i])
		{
			OrderFailed();
			return;
		}
	}
	OrderCompleted();
}

void AG2IActivationOrderManager::LockedCorrectOrderActors(UG2ILauncherComponent* LauncherComponent,
	AActor* ComponentOwner, const bool bIsLocked)
{
	for (AActor* Actor : CorrectOrderOfActors)
	{
		if (!ensure(Actor))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: In ActivatedActorsArray actor is null"), *GetActorNameOrLabel());
			continue;
		}
		if (Actor->Implements<UG2ILockingInterface>())
		{
			IG2ILockingInterface::Execute_SetIsLocked(Actor, bIsLocked);
		}
	}
}
