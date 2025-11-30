#include "G2IPlayerState.h"

#include "AIController.h"
#include "G2I.h"
#include "DataTables/G2IItemCharacter.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

void AG2IPlayerState::BeginPlay()
{
	Super::BeginPlay();

	SetupPlayableDataTable();
	SetupPlayableCharacters();
}

void AG2IPlayerState::SetupPlayableDataTable()
{
	if (PlayableCharactersDataTable)
	{
		if (PlayableCharactersDataTable->GetRowStruct() == FG2IItemCharacter::StaticStruct())
		{
			PlayableCharactersRowNames = PlayableCharactersDataTable->GetRowNames();
		}
		else
		{
			UE_LOG(LogG2I, Error, TEXT("In %s in %s table rows isn`t target type"),
				*GetName(), *PlayableCharactersDataTable.GetName());
		}
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("In %s doesn't have the table listing playable characters"), *GetName());
	}
}

void AG2IPlayerState::SetupPlayableCharacters()
{
	if (!PlayableCharactersRowNames.IsEmpty())
	{
		for (int32 NumberRowName = 0; NumberRowName < PlayableCharactersRowNames.Num(); ++NumberRowName)
		{
			SetupControllerForPawn(NumberRowName);
		}
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("Array of playable characters is empty"));
	}
}

AActor *AG2IPlayerState::FindOneActor(const TSubclassOf<AActor> ActorClass) const
{
	AActor *TargetActor = nullptr;
	if (const UWorld *World = GetWorld())
	{
		if (ActorClass)
		{
			TArray<AActor *> AllActorTargetClass;
			UGameplayStatics::GetAllActorsOfClass(World, ActorClass, AllActorTargetClass);
			if (!AllActorTargetClass.IsEmpty())
			{
				if (AllActorTargetClass.Num() > 1)
				{
					UE_LOG(LogG2I, Log, TEXT("%s this more then one actor of this class in the World."
							   "Choose first actor of this class"), *ActorClass->GetName());
				}
				TargetActor = AllActorTargetClass[0];
			}
		}
		else
		{
			UE_LOG(LogG2I, Log, TEXT("Looking for nullptr class"));
		}
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist"));
	}
	
	if (TargetActor == nullptr)
	{
		UE_LOG(LogG2I, Log, TEXT("Actor doesn't founded in World"));
	}
	else
	{
		UE_LOG(LogG2I, Log, TEXT("Actor %s was founded"), *TargetActor->GetName());
	}
	
	return TargetActor;
}

AActor *AG2IPlayerState::SpawnActor(const TSubclassOf<AActor> ActorClass) const
{
	AActor *TargetActor = nullptr;
	if (UWorld *World = GetWorld())
	{
		if (ActorClass)
		{
			const FActorSpawnParameters SpawnInfo;
			TargetActor = World->SpawnActor<AAIController>(ActorClass, FVector::ZeroVector,
				FRotator::ZeroRotator, SpawnInfo);
			UE_LOG(LogG2I, Log, TEXT("Actor %s was created"), *TargetActor->GetName());
		}
		else
		{
			UE_LOG(LogG2I, Log, TEXT("Looking for nullptr actor class in %s"), *GetName());
		}
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
	}

	if (TargetActor == nullptr)
	{
		UE_LOG(LogG2I, Error, TEXT("Actor doesn't created in %s"), *GetName());
	}

	return TargetActor;
}

bool AG2IPlayerState::SetupPlayerControllerForPawn(const uint32 PawnNumber, APawn& CurrentPawn)
{
	if (const UWorld *World = GetWorld())
	{
		if (APlayerController *CurrentPlayerController = World->GetFirstPlayerController())
		{
			NumberCurrentCharacter = PawnNumber;
			CurrentPlayerController->Possess(&CurrentPawn);
			return true;
		}
		else
		{
			UE_LOG(LogG2I, Error, TEXT("PlayerController doesn't exist"));
		}
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist"));
	}
	return false;
}

bool AG2IPlayerState::SetupControllerForPawn(const uint32 PawnNumber, AAIController& CurrentAIController,
	APawn& CurrentPawn)
{
	if (NumberCurrentCharacter != PawnNumber)
	{
		CurrentAIController.Possess(&CurrentPawn);
		return true;
	}
	else
	{
		return SetupPlayerControllerForPawn(PawnNumber, CurrentPawn);
	}
}

bool AG2IPlayerState::SetupControllerForPawn(const uint32 PawnNumber, const TSubclassOf<AActor> AIControllerActorClass,
	APawn& CurrentPawn)
{
	if (NumberCurrentCharacter != PawnNumber)
	{
		if (AIControllerActorClass)
		{
			AActor *AIControllerActor = FindOneActor(AIControllerActorClass);
			if (AIControllerActor == nullptr)
			{
				UE_LOG(LogG2I, Log, TEXT("The ai controller from %i row data table of playable"
					  "characters doesn't founded"), PawnNumber);
				AIControllerActor = SpawnActor(AIControllerActorClass);
			}
			if (AAIController *TargetAIController = Cast<AAIController>(AIControllerActor))
			{
				return SetupControllerForPawn(PawnNumber, *TargetAIController, CurrentPawn);
			}
			else
			{
				UE_LOG(LogG2I, Log, TEXT("The target ai controller doesn't created or founded for"
					"%i row in data table of playable character"), PawnNumber);
			}
		}
		else
		{
			UE_LOG(LogG2I, Warning, TEXT("Actor class for ai controller is unabled in %i row"), PawnNumber);
		}
	}
	else
	{
		return SetupPlayerControllerForPawn(PawnNumber, CurrentPawn);
	}
	return false;
}

bool AG2IPlayerState::SetupControllerForPawn(const uint32 PawnNumber) 
{
	if (const FG2IItemCharacter *Row = PlayableCharactersDataTable->FindRow<FG2IItemCharacter>(
		PlayableCharactersRowNames[PawnNumber], TEXT("Playable Character Context")))
	{
		if (APawn *PlayablePawn = GetPawn(PawnNumber))
		{
			return SetupControllerForPawn(PawnNumber, Row->AIControllerClass, *PlayablePawn);
		}
		else
		{
			UE_LOG(LogG2I, Warning, TEXT("Pawn doesn't exist"));
		}
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("In %s array of row names doesn't sync with %s"), *GetName(),
			*PlayableCharactersDataTable.GetName());
	}
	return false;
}

void AG2IPlayerState::SelectNextCharacter()
{
	if (!PlayableCharactersRowNames.IsEmpty())
	{
		for (int32 OffsetRowName = 1; OffsetRowName <= PlayableCharactersRowNames.Num(); ++OffsetRowName)
		{
			const int32 NewCharacterIndex = (NumberCurrentCharacter + OffsetRowName) %
				PlayableCharactersRowNames.Num();
			if (NewCharacterIndex == NumberCurrentCharacter)
			{
				UE_LOG(LogG2I, Log, TEXT("Character doesn't switched"));
				return;
			}

			const int32 OldCharacterNumber = NumberCurrentCharacter;
			NumberCurrentCharacter = NewCharacterIndex;
			if (SetupControllerForPawn(OldCharacterNumber))
			{
				if (SetupControllerForPawn(NumberCurrentCharacter))
				{
					OnNewControllerPossessDelegate.Broadcast(GetPawn(OldCharacterNumber));
					OnNewControllerPossessDelegate.Broadcast(GetPawn(NumberCurrentCharacter));
					break;
				}
				else
				{
					UE_LOG(LogG2I, Warning, TEXT("Character %i doesn't switched on player controller"),
						NumberCurrentCharacter);
					NumberCurrentCharacter = OldCharacterNumber;
					check(!SetupControllerForPawn(NumberCurrentCharacter));
				}
			}
			else
			{
				UE_LOG(LogG2I, Warning, TEXT("Character %i doesn't switched on ai controller"),
					OldCharacterNumber);
				NumberCurrentCharacter = OldCharacterNumber;
			}
		}
	}
}

APawn *AG2IPlayerState::GetPawn(const uint32 PawnNumber)
{
	if (const FG2IItemCharacter *Row = PlayableCharactersDataTable->FindRow<FG2IItemCharacter>(
	PlayableCharactersRowNames[PawnNumber], TEXT("Playable Character Context")))
	{
		if (AActor *PlayableActor = FindOneActor(Row->CharacterClass))
		{
			if (APawn *PlayablePawn = Cast<APawn>(PlayableActor))
			{
				return PlayablePawn;
			}
			else
			{
				UE_LOG(LogG2I, Warning, TEXT("%s doesn't pawn"), *PlayableActor->GetName());
			}
		}
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("In %s array of row names doesn't sync with %s"), *GetName(),
			*PlayableCharactersDataTable.GetName());
	}
	return nullptr;
}