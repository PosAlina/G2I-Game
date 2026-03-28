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
	SetCurrentLevelName();
	if (IsMainMenuLevel())
	{
		UE_LOG(LogG2I, Log, TEXT("Open Main Menu"));
	}
	else
	{
		UE_LOG(LogG2I, Log, TEXT("Open level %s (%d index)"), *CurrentLevelName, CurrentLevelIndex);
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
	CurrentLevelName = World->GetMapName().Mid(World->StreamingLevelsPrefix.Len());
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
		LoadMainMenuLevel();
	}
}

void UG2IGameInstance::LoadMainMenuLevel()
{
	const int32 OldLevelIndex = CurrentLevelIndex;
	SetMainMenuLevelIndex();
	
	if (!OpenLevel(MainMenuLevel))
	{
		CurrentLevelIndex = OldLevelIndex;
	}
}

void UG2IGameInstance::SetMainMenuLevelIndex()
{
	CurrentLevelIndex = -1;
}

bool UG2IGameInstance::OpenLevel(const TSoftObjectPtr<UWorld>& Level) const
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
	return true;
}
