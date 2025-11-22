#include "G2IPlayerState.h"
#include "G2I.h"
#include "DataTables/G2IItemCharacter.h"
#include "Kismet/GameplayStatics.h"

void AG2IPlayerState::BeginPlay()
{
	Super::BeginPlay();

	SetupPlayableDataTable();
	SetupDefaultCharacter();
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

void AG2IPlayerState::SetupDefaultCharacter()
{
	const FVector NewCharacterLocationIfUnexisted = FVector(0, 0, 0);
	const FRotator NewCharacterRotationIfUnexisted = FRotator(0, 0, 0);
	SwitchCharacters(NumberCurrentCharacter, NewCharacterLocationIfUnexisted, NewCharacterRotationIfUnexisted);
}

void AG2IPlayerState::SpawnCharacter(const uint32 NewCharacterNumber, const TSubclassOf<AActor> CharacterClass,
	const FVector& CharacterSpawnLocation, const FRotator& CharacterSpawnRotation)
{
	if (CharacterClass)
	{
		if (UWorld *World = GetWorld())
		{
			const FActorSpawnParameters SpawnInfo;
			if (APawn *NewCharacter = World->SpawnActor<APawn>(CharacterClass, CharacterSpawnLocation,
				CharacterSpawnRotation, SpawnInfo))
			{
				SelectCharacter(NewCharacterNumber, *NewCharacter);
			}
			else
			{
				UE_LOG(LogG2I, Warning, TEXT("New character couldn't created"));
			}
		}
		else
		{
			UE_LOG(LogG2I, Error, TEXT("World doesn't exist"));
		}
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("It is attempt to spawn character of an unexisting class"));
	}
}

void AG2IPlayerState::SelectCharacter(const uint32 NewCharacterNumber, APawn &NewCharacter)
{
	if (const UWorld *World = GetWorld())
	{
		if (APlayerController *CurrentPlayerController = World->GetFirstPlayerController())
		{
			NumberCurrentCharacter = NewCharacterNumber;
			CurrentPlayerController->Possess(&NewCharacter);
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
}

void AG2IPlayerState::SelectNextCharacter(const FVector& CharacterSpawnLocation, const FRotator& CharacterSpawnRotation)
{
	SwitchCharacters(NumberCurrentCharacter + 1, CharacterSpawnLocation, CharacterSpawnRotation);
}

void AG2IPlayerState::SwitchCharacters(const uint32 NewCharacterNumber,
	const FVector& CharacterSpawnLocation, const FRotator& CharacterSpawnRotation)
{
	if (!PlayableCharactersRowNames.IsEmpty())
	{
		const uint32 NewCharacterIndex = NewCharacterNumber % PlayableCharactersRowNames.Num();
		const FName NewCharacterRowName = PlayableCharactersRowNames[NewCharacterIndex];
		if (FG2IItemCharacter *Row = PlayableCharactersDataTable->FindRow<FG2IItemCharacter>(
			NewCharacterRowName, TEXT("Playable Character Context")))
		{
			if (const UWorld *World = GetWorld())
			{
				if (const TSubclassOf<AActor> NewCharacterClass = Row->CharacterClass)
				{
					TArray<AActor *> AllCharacterCurrentClass;
					UGameplayStatics::GetAllActorsOfClass(World, NewCharacterClass, AllCharacterCurrentClass);
					if (!AllCharacterCurrentClass.IsEmpty())
					{
						if (AllCharacterCurrentClass.Num() > 1)
						{
							UE_LOG(LogG2I, Warning, TEXT("%s this more then one actor of this class in the World."
									   "Choose first actor of this class"), *NewCharacterClass->GetName());
						}
						if (APawn *NewPawn = Cast<APawn>(AllCharacterCurrentClass[0]))
						{
							SelectCharacter(NewCharacterNumber, *NewPawn);
						}
						else
						{
							UE_LOG(LogG2I, Error, TEXT("It is attemp to posses on not pawn actor"));
						}
					}
					else
					{
						UE_LOG(LogG2I, Log, TEXT("%s doesn't exist in the world. So it's spawn"),
							*NewCharacterClass->GetName());
						SpawnCharacter(NewCharacterNumber, NewCharacterClass, CharacterSpawnLocation, CharacterSpawnRotation);
					}
				}
				else
				{
					UE_LOG(LogG2I, Warning, TEXT("In %s doesn't determine character class in row"),
						*PlayableCharactersDataTable.GetName());
				}
			}
			else
			{
				UE_LOG(LogG2I, Error, TEXT("World doesn't exist"));
			}
		}
		else
		{
			UE_LOG(LogG2I, Error, TEXT("In %s array of row names doesn't sync with %s"), *GetName(),
				*PlayableCharactersDataTable.GetName());
		}
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("Array of playable characters is empty"));
	}
}