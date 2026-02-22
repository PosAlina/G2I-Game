
#include "Gameplay/G2IActivationOrderManager.h"
#include "G2I.h"

AG2IActivationOrderManager::AG2IActivationOrderManager()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	if (RootSceneComponent)
		SetRootComponent(RootSceneComponent);
}

void AG2IActivationOrderManager::OrderCompleted()
{
	UE_LOG(LogG2I, Log, TEXT("Order completed successfully in %s"), *GetActorNameOrLabel());
}

void AG2IActivationOrderManager::OrderFailed()
{
	CurrentIndex = 0;
	for (AActor* Actor : ActivatedActorsArray)
	{
		if (auto* ActivationComponent = Actor->GetComponentByClass<UG2IActivationWithOrderComponent>())
			ActivationComponent->Restored();
	}
	ActivatedActorsArray.Empty();
	UE_LOG(LogG2I, Warning, TEXT("Order failed in %s"), *GetActorNameOrLabel());
}

void AG2IActivationOrderManager::OrderCancelled()
{
	CurrentIndex = 0;
	for (AActor* Actor : ActivatedActorsArray)
	{
		if (auto* ActivationComponent = Actor->GetComponentByClass<UG2IActivationWithOrderComponent>())
			ActivationComponent->Restored();
	}
	ActivatedActorsArray.Empty();
	UE_LOG(LogG2I, Warning, TEXT("Order calcelled in %s"), *GetActorNameOrLabel());
}

void AG2IActivationOrderManager::BeginPlay()
{
	Super::BeginPlay();
	
	BindToAllDelegates();
	NumberOfActors = ActorsActivatingArray.Num();
	ActivatedActorsArray.Reserve(NumberOfActors);
}

void AG2IActivationOrderManager::BindToAllDelegates()
{
	for (AActor* Actor : ActorsActivatingArray)
	{
		if (auto* ActivationComponent = Actor->GetComponentByClass<UG2IActivationWithOrderComponent>())
		{
			ActivationComponent->OnActivatedDelegate.BindUObject(this, &AG2IActivationOrderManager::OnActorActivated);
		}
		else
			UE_LOG(LogG2I, Error, TEXT("Failed to get ActivationWithOrder component from %s actor in %s"), *Actor->GetActorNameOrLabel(), *GetActorNameOrLabel());
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
	if (CurrentIndex + 1 < NumberOfActors)
		return;

	for (int32 i = 0; i < NumberOfActors; i++)
	{
		if (ActorsActivatingArray[i] != ActivatedActorsArray[i])
		{
			OrderFailed();
			return;
		}
	}
	OrderCompleted();
}
