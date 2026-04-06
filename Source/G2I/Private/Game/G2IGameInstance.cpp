#include "G2IGameInstance.h"
#include "G2I.h"
#include "G2IUIManager.h"
#include "G2IWidgetNames.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
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
	for (const auto& [Name, Level] : Levels)
	{
		if (Level.GetAssetName() == LevelName)
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

	//TODO: Loading Screen By Time
	/*
	if (LoadScreenLoading())
	{
		UE_LOG(LogG2I, Log, TEXT("%s: Create Loading Screen"), *GetName());
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Can't create Loading Screen"), *GetName());
	}

	LoadingLevelStreamingHandle =
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
		Level.GetLongPackageName(), FStreamableDelegate());

	const FTimerDelegate Delegate =
		FTimerDelegate::CreateUObject(
			this, &ThisClass::UpdateLoadingProgress, FName(Level.GetAssetName()));
	World->GetTimerManager().SetTimer(LoadingTimerHandle, Delegate, .1f, true);
	*/

	UGameplayStatics::OpenLevel(World, FName(LevelName));
	SetCurrentLevelInfo();
	return true;
}

bool UG2IGameInstance::LoadScreenLoading() const
{
	const UG2IUIManager *UIManager = GetSubsystem<UG2IUIManager>();
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return false;
	}
	UIManager->OpenWidget(EG2IWidgetNames::LevelLoadingScreen, false);
	return true;
}

void UG2IGameInstance::UpdateLoadingProgressFixTime(const FName LevelName)
{
	++TimeCount;
	
	const UG2IUIManager *UIManager = GetSubsystem<UG2IUIManager>();
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
	}
	else
	{
		if (MaxTimeCount != 0)
		{
			const float Percentage = TimeCount / MaxTimeCount;
			UIManager->SetLoadingProgressPercent(Percentage);
		}
	}
	
	if (TimeCount >= MaxTimeCount)
	{
		const UWorld *World = GetWorld();
		if (!ensure(World))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: World is null"), *GetName());
			return;
		}
		
		World->GetTimerManager().ClearTimer(LoadingTimerHandle);
		UGameplayStatics::OpenLevel(World, LevelName);
		UIManager->CloseWidget(EG2IWidgetNames::LevelLoadingScreen);
		SetCurrentLevelInfo();
		TimeCount = 0;
		return;
	}
}

void UG2IGameInstance::UpdateLoadingProgress(const FName LevelName)
{
	if (!LoadingLevelStreamingHandle.IsValid())
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Not valid LoadingLevelStreamingHandle"), *GetName());
		FinishLoading(LevelName);
		return;
	}
	const float LoadingProgress = LoadingLevelStreamingHandle->GetProgress();
	// TODO: Move UIManager initialization in start game
	const UG2IUIManager *UIManager = GetSubsystem<UG2IUIManager>();
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
	}
	else
	{
		UIManager->SetLoadingProgressPercent(LoadingProgress);
	}
	
	if (LoadingProgress >= 1.f)
	{
		UE_LOG(LogG2I, Log, TEXT("%s: Full progress in Loading"), *GetName());
		FinishLoading(LevelName);
		return;
	}
}

void UG2IGameInstance::FinishLoading(const FName LevelName)
{
	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: World is null"), *GetName());
		return;
	}
	UGameplayStatics::OpenLevel(World, FName(LevelName));
	World->GetTimerManager().ClearTimer(LoadingTimerHandle);

	if (!ensure(LevelName != NAME_None))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Attempt to open null level"), *GetName());
		return;
	}

	if (LoadingLevelStreamingHandle->HasError())
	{
		UE_LOG(LogG2I, Error, TEXT("%s: LoadingLevelStreamingHandle has error"), *GetName());
	}
	UE_LOG(LogG2I, Log, TEXT("%s: Attempt to open level %s"), *GetName(), *LevelName.ToString());
	//UGameplayStatics::OpenLevelBySoftObjectPtr(World, LoadingLevelStreamingHandle->GetLoadedAsset());
	UGameplayStatics::OpenLevel(World, LevelName);
	SetCurrentLevelInfo();
}