#include "G2IInvisibleForCharactersComponent.h"
#include "G2I.h"
#include "G2IPlayerController.h"
#include "GameFramework/Character.h"

void UG2IInvisibleForCharactersComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupDefaults();
	BindingToDelegates();
}

void UG2IInvisibleForCharactersComponent::BindingToDelegates()
{
	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}
	
	APlayerController *FirstPlayerController = World->GetFirstPlayerController();
	if (!ensure(FirstPlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller doesn't exist in %s"), *GetName());
		return;
	}

	AG2IPlayerController *PlayerController = Cast<AG2IPlayerController>(FirstPlayerController);
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller isn't target %s in %s"),
			*AG2IPlayerController::StaticClass()->GetName(), *GetName());
		return;
	}

	PlayerController->OnPossessPawnDelegate.AddDynamic(this,
		&UG2IInvisibleForCharactersComponent::HideActorsForCharacter);
	PlayerController->OnUnPossessPawnDelegate.AddDynamic(this,
		&UG2IInvisibleForCharactersComponent::ShowActorsForCharacter);
}

void UG2IInvisibleForCharactersComponent::SetupDefaults()
{
	Owner = GetOwner();
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s of %s"), *GetName(),
			*Owner->GetActorNameOrLabel());
		return;
	}
}

void UG2IInvisibleForCharactersComponent::HideActorsForCharacter(APawn *Pawn)
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s of %s"), *GetName(),
			*Owner->GetActorNameOrLabel());
		return;
	}

	if (!Pawn)
	{
		UE_LOG(LogG2I, Warning, TEXT("Attempting to hide %s for nullptr pawn"), *Owner->GetActorNameOrLabel());
		return;
	}
	
	for (TSubclassOf CharacterClass : InvisibleFromCharacters)
	{
		if (Pawn->GetClass() == CharacterClass.Get())
		{
			Owner->SetActorEnableCollision(false);
			Owner->SetActorHiddenInGame(true);
			break;
		}
	}
}

void UG2IInvisibleForCharactersComponent::ShowActorsForCharacter(APawn *Pawn)
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s of %s"), *GetName(),
			*Owner->GetActorNameOrLabel());
		return;
	}

	if (!Pawn)
	{
		UE_LOG(LogG2I, Error, TEXT("Attempting to show %s for nullptr pawn"), *Owner->GetActorNameOrLabel());
		return;
	}
	
	for (TSubclassOf CharacterClass : InvisibleFromCharacters)
	{
		if (Pawn->GetClass() == CharacterClass.Get())
		{
			Owner->SetActorEnableCollision(true);
			Owner->SetActorHiddenInGame(false);
			break;
		}
	}
}
