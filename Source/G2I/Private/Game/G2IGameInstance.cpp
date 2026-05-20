#include "G2IGameInstance.h"
#include "G2I.h"
#include "Kismet/GameplayStatics.h"
#include "AsyncLoadingScreenLibrary.h"
#include "G2IOptionsParameters.h"
#include "G2IUIManager.h"
#include "G2IWidgetNames.h"
#include "Public/Game/G2ITaskManager.h"
#include "G2ISavingGameplayManager.h"

void UG2IGameInstance::Init()
{
	Super::Init();
	
	MainMenuLevelName = MainMenuLevelInfo.LevelAsset.GetAssetName();
	if (!ensure(!MainMenuLevelName.IsEmpty()))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Main Menu Level isn't set"), *GetName());
	}
	if (!ensure(OptionsParameters))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
			*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
	}
	else
	{
		OptionsParameters->SetupDefaultParameters();
		// TODO: Setup Saved Parameters if they exist
	}
	
	if (UG2IGameSoundManager* SoundManager = GetSubsystem<UG2IGameSoundManager>())
	{
		TMap<EG2ISoundType, USoundClass*> TempSoundClasses;

		for (const auto& Pair : DefaultSoundClasses)
		{
			TempSoundClasses.Add(Pair.Key, Pair.Value.Get());
		}
		SoundManager->InitGlobalAudio(DefaultMainSoundMix.Get(), TempSoundClasses);

	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
			*GetName(), *UG2IGameSoundManager::StaticClass()->GetName());
	}

	OnPlayerControllerInitDelegate.AddUObject(this, &ThisClass::StartLevelInitialize);
}

void UG2IGameInstance::StartLevelInitialize()
{
	SetCurrentLevelInfo();
	OnStartLevelInitDelegate.Broadcast();

	if (CurrentLevelEnum == EG2ILevelName::BoilerRoom)
	{
		UG2ITaskManager *TaskManager = GetSubsystem<UG2ITaskManager>();
		if (!ensure(TaskManager))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(), *UG2ITaskManager::StaticClass()->GetName());
			return;
		}
		TaskManager->CreateTaskSwitchCharacter();
	}
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
		CurrentLevelIndex = GetIndex(CurrentLevelEnum);
		if (CurrentLevelIndex == INDEX_NONE)
		{
			UE_LOG(LogG2I, Log, TEXT("Open unregistered Level %s"), *CurrentLevelName);
			return;
		}
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

UG2ICutScenesParameters* UG2IGameInstance::GetCutScenesParameters()
{
	return CutScenesParameters;
}

UG2IOptionsParameters* UG2IGameInstance::GetOptionsParameters()
{
	return OptionsParameters;
}

UG2ITasksCatalog* UG2IGameInstance::GetTasksCatalog()
{
	return TasksCatalog;
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
	return LevelsNameInOrderInGame.Find(LevelName);
#endif
}

EG2ILevelName UG2IGameInstance::GetLevelEnum(const FString& LevelName) const
{
	for (const auto& [Name, LevelInfo] : LevelsInfo)
	{
		if (LevelInfo.LevelAsset.GetAssetName() == LevelName)
		{
			return Name;
		}
	}
	return EG2ILevelName::None;
}

EG2ILevelName UG2IGameInstance::GetCurrentLevelEnum() const
{
	return CurrentLevelEnum;
}

bool UG2IGameInstance::IsMainMenuLevel() const
{
	return MainMenuLevelName == CurrentLevelName;
}

bool UG2IGameInstance::LoadLevel(const EG2ILevelName& LevelName)
{
	const FG2ILevelInfo *LevelInfo = LevelsInfo.Find(LevelName);
	if (!ensure(LevelInfo))
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
	
	return LoadLevel(*LevelInfo, NewLevelIndex);
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

	const FG2ILevelInfo *LevelInfo = LevelsInfo.Find(NewLevelName);
	if (!ensure(LevelInfo))
	{
		UE_LOG(LogG2I, Log, TEXT("%s: Couldn't find level"), *GetName());
		return false;
	}
	return LoadLevel(*LevelInfo, Index);
}

bool UG2IGameInstance::LoadLevel(const FG2ILevelInfo& LevelInfo, const uint32 Index)
{
	const TSoftObjectPtr<UWorld> Level = LevelInfo.LevelAsset;
	UAsyncLoadingScreenLibrary::SetDisplayBackgroundIndex(LevelInfo.BackgroundIndex);
	
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
#if WITH_EDITOR
	int32 LastLevelIndex = LevelsNameInOrderInEditor.Num() - 1;
#else
	int32 LastLevelIndex = LevelsNameInOrderInGame.Num() - 1;
#endif
	
	if (CurrentLevelIndex == LastLevelIndex)
	{
		const UG2IUIManager *UIManager = GetSubsystem<UG2IUIManager>();
		if (!ensure(UIManager))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
				*UG2IUIManager::StaticClass()->GetName());
			return false;
		}
		UIManager->OpenWidget(EG2IWidgetNames::CutSceneEndGame);
		return true;
	}
	return LoadLevel(CurrentLevelIndex + 1);
}

bool UG2IGameInstance::LoadMainMenuLevel()
{
	const int32 OldLevelIndex = CurrentLevelIndex;
	SetMainMenuLevelIndex();

	const TSoftObjectPtr<UWorld> MainMenuLevel = MainMenuLevelInfo.LevelAsset;
	UAsyncLoadingScreenLibrary::SetDisplayBackgroundIndex(MainMenuLevelInfo.BackgroundIndex);
	
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
	const FString& LevelName = Level.GetAssetName();
	if (!ensure(!LevelName.IsEmpty()))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Attempt to open null level"), *GetName());
		return false;
	}
	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: World is null"), *GetName());
		return false;
	}
	OnCloseLevelDelegate.Broadcast();
	
	UGameplayStatics::OpenLevel(World, FName(LevelName));
	SetCurrentLevelInfo();

	if (!IsMainMenuLevel())
	{
		UG2ISavingGameplayManager* SaveManager = GetSubsystem<UG2ISavingGameplayManager>();
		if (!ensure(SaveManager))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldnn't find %s"), *GetName(), *UG2ISavingGameplayManager::StaticClass()->GetName());
			return true;
		}
		SaveManager->SaveCurrentLevel(CurrentLevelEnum, true);
	}
	
	return true;
}