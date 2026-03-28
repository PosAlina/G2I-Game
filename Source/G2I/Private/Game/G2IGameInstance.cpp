#include "G2IGameInstance.h"
#include "G2I.h"
#include "Kismet/GameplayStatics.h"

void UG2IGameInstance::Init()
{
	Super::Init();
	
	MainMenuLevelName = MainMenuLevel.GetAssetName();
	if (!ensure(!MainMenuLevelName.IsEmpty()))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Main Menu Level isn't set"), *GetName());
	}
	OnPlayerControllerInitDelegate.AddUObject(this, &ThisClass::StartLevelInitialize);
}

void UG2IGameInstance::StartLevelInitialize()
{
	SetCurrentLevelInfo();
	OnStartLevelInitDelegate.Broadcast();
}

void UG2IGameInstance::SetCurrentLevelInfo()
{
	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: World is null"), *GetName());
	}
	CurrentLevelName = World->GetMapName().Mid(World->StreamingLevelsPrefix.Len());

	if (IsMainMenuLevel())
	{
		CurrentLevelIndex = -1;
		CurrentLevelEnum = EG2ILevelName::None;
		UE_LOG(LogG2I, Log, TEXT("Open Main Menu"));
	}
	else
	{
		CurrentLevelEnum = GetLevelEnum(CurrentLevelName);
		if (!ensure(CurrentLevelEnum != EG2ILevelName::None))
		{
			UE_LOG(LogG2I, Warning,
				TEXT("Attempted to open unregistered Level %s. Opening main menu instead."), *CurrentLevelName);
			LoadMainMenuLevel();
			return;
		}
		CurrentLevelIndex = GetIndex(CurrentLevelEnum);
		UE_LOG(LogG2I, Log, TEXT("Open level %s (%d index)"), *CurrentLevelName, CurrentLevelIndex);
	}
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

FString UG2IGameInstance::GetMainMenuLevelName() const
{
	return MainMenuLevelName;
}

FString UG2IGameInstance::GetCurrentLevelName() const
{
	return CurrentLevelName;
}

int32 UG2IGameInstance::GetIndex(const EG2ILevelName& LevelName) const
{
#if WITH_EDITOR
	return LevelsNameInOrderInEditor.Find(LevelName);
#else
	return NewLevelIndex = LevelsNameInOrderInGame.Find(LevelName);
#endif
}

EG2ILevelName UG2IGameInstance::GetLevelEnum(const FString& LevelName) const
{
	for (const auto& [Name, Level] : Levels)
	{
		if (Level.GetAssetName() == LevelName)
		{
			return Name;
		}
	}
	return EG2ILevelName::None;
}

bool UG2IGameInstance::IsMainMenuLevel() const
{
	return MainMenuLevelName == CurrentLevelName;
}

bool UG2IGameInstance::LoadLevel(const EG2ILevelName& LevelName)
{
	const TSoftObjectPtr<UWorld> *Level = Levels.Find(LevelName);
	if (!ensure(*Level))
	{
		UE_LOG(LogG2I, Log, TEXT("%s: Couldn't find level"), *GetName());
		return false;
	}
	const int32 NewLevelIndex = GetIndex(LevelName);
	if (!ensure(NewLevelIndex != INDEX_NONE))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Not level in array of levels in order"), *GetName());
		return false;
	}
	
	return LoadLevel(*Level, NewLevelIndex);
}

bool UG2IGameInstance::LoadLevel(uint32 Index)
{
#if WITH_EDITOR
	if (!LevelsNameInOrderInEditor.IsValidIndex(Index))
#else
	if (!LevelsNameInOrderInGame.IsValidIndex(Index))
#endif
	{
		// If new index is too large
		UE_LOG(LogG2I, Log, TEXT("%s: Couldn't find %d level. Open main menu"), *GetName(), Index);
		return LoadMainMenuLevel();
	}

#if WITH_EDITOR
	const EG2ILevelName& NewLevelName = LevelsNameInOrderInEditor[Index];
#else
	const EG2ILevelName& NewLevelName = LevelsNameInOrderInGame[Index];
#endif

	const TSoftObjectPtr<UWorld> *Level = Levels.Find(NewLevelName);
	if (!ensure(Level))
	{
		UE_LOG(LogG2I, Log, TEXT("%s: Couldn't find %d level"), *GetName(), Index);
		return false;
	}
	return LoadLevel(*Level, Index);
}

bool UG2IGameInstance::LoadLevel(const TSoftObjectPtr<UWorld>& Level, const uint32 Index)
{
	const int32 OldLevelIndex = CurrentLevelIndex;
	CurrentLevelIndex = Index;
	
	if (!OpenLevel(Level))
	{
		CurrentLevelIndex = OldLevelIndex;
		return false;
	}
	return true;
}

bool UG2IGameInstance::LoadNextLevel()
{
	if (!LoadLevel(CurrentLevelIndex + 1))
	{
		return LoadMainMenuLevel();
	}
	return true;
}

bool UG2IGameInstance::LoadMainMenuLevel()
{
	const int32 OldLevelIndex = CurrentLevelIndex;
	SetMainMenuLevelIndex();
	
	if (!OpenLevel(MainMenuLevel))
	{
		CurrentLevelIndex = OldLevelIndex;
		return false;
	}
	return true;
}

void UG2IGameInstance::SetMainMenuLevelIndex()
{
	CurrentLevelIndex = -1;
}

bool UG2IGameInstance::OpenLevel(const TSoftObjectPtr<UWorld>& Level)
{
	if (!ensure(!Level.GetAssetName().IsEmpty()))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Attempt to open null level"), *GetName());
	}
	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: World is null"), *GetName());
		return false;
	}
	OnCloseLevelDelegate.Broadcast();
	UGameplayStatics::OpenLevel(World, FName(Level.GetAssetName()));
	SetCurrentLevelInfo();
	return true;
}
