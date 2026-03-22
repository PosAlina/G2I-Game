#include "G2IGameInstance.h"
#include "G2I.h"
#include "G2IPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UG2IGameInstance::Init()
{
	Super::Init();

	OnPlayerControllerInitDelegate.AddDynamic(this, &ThisClass::StartLevelInitialize);
}

void UG2IGameInstance::StartLevelInitialize()
{
	SetCurrentLevelName();
	if (IsMainMenuLevel())
	{
		UE_LOG(LogG2I, Log, TEXT("Open Main Menu"));
	}
	else
	{
		UE_LOG(LogG2I, Log, TEXT("Open level %s (%d index)"), *CurrentLevelName.ToString(), CurrentLevelIndex);
	}
	OnStartLevelInitDelegate.Broadcast();
}

void UG2IGameInstance::SetCurrentLevelName()
{
	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: World is null"), *GetName());
	}
	CurrentLevelName = FName(World->GetMapName().Mid(World->StreamingLevelsPrefix.Len()));
}

UG2IWidgetsCatalog* UG2IGameInstance::GetWidgetsCatalog()
{
	return WidgetsCatalog;
}

UG2IStringTablesCatalog* UG2IGameInstance::GetStringTablesCatalog()
{
	return StringTablesCatalog;
}

UG2IWidgetComponentParameters* UG2IGameInstance::GetWidgetComponentParameters()
{
	return WidgetComponentsParameters;
}

FName UG2IGameInstance::GetMainMenuLevelName() const
{
	return MainMenuLevelName;
}

FName UG2IGameInstance::GetCurrentLevelName() const
{
	return CurrentLevelName;
}

bool UG2IGameInstance::IsMainMenuLevel() const
{
	return MainMenuLevelName == CurrentLevelName;
}

bool UG2IGameInstance::LoadLevelByIndex(uint32 Index)
{ 
#if WITH_EDITOR
	uint32 MaxLevel = LevelsNameInOrderInEditor.Num();
#else
	uint32 MaxLevel = LevelsNameInOrderInGame.Num();
#endif

	if (Index >= MaxLevel)
	{
		UE_LOG(LogG2I, Log, TEXT("%s: Couldn't find %d level. Max level - %d."), *GetName(), Index, MaxLevel);
		return false;
	}

	const int32 OldLevelIndex = CurrentLevelIndex;
	CurrentLevelIndex = Index;
	
#if WITH_EDITOR
	if (!OpenLevel(LevelsNameInOrderInEditor[Index]))
#else
	if (!OpenLevel(LevelsNameInOrderInGame[Index]))
#endif
	{
		CurrentLevelIndex = OldLevelIndex;
	}

	return true;
}

void UG2IGameInstance::LoadNextLevel()
{
	if (!LoadLevelByIndex(CurrentLevelIndex + 1))
	{
		const UWorld *World = GetWorld();
		if (!ensure(World))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: World is null"), *GetName());
			return;
		}
		APlayerController *LocalPlayerController = World->GetFirstPlayerController();
		if (!ensure(LocalPlayerController))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Local player controller is null"), *GetName());
			return;
		}
		AG2IPlayerController *PlayerController = Cast<AG2IPlayerController>(LocalPlayerController);
		if (!ensure(PlayerController))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
				*AG2IPlayerController::StaticClass()->GetName());
			return;
		}
		PlayerController->QuitGame();
	}
}

void UG2IGameInstance::LoadMainMenuLevel()
{
	const int32 OldLevelIndex = CurrentLevelIndex;
	SetMainMenuLevelIndex();
	
	if (!OpenLevel(MainMenuLevelName))
	{
		CurrentLevelIndex = OldLevelIndex;
	}
}

void UG2IGameInstance::SetMainMenuLevelIndex()
{
	CurrentLevelIndex = -1;
}

bool UG2IGameInstance::OpenLevel(const FName LevelName) const
{
	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: World is null"), *GetName());
		return false;
	}

	OnCloseLevelDelegate.Broadcast();
	UGameplayStatics::OpenLevel(World, LevelName);

	return true;
}
