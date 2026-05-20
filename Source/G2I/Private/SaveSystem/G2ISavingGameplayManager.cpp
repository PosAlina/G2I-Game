#include "G2ISavingGameplayManager.h"
#include "G2ISavableInterface.h"
#include "G2I.h"


void UG2ISavingGameplayManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    OnGameplayAsyncSavedDelegate.BindUObject(this, &UG2ISavingGameplayManager::OnGameplayAsyncSaved);
    OnGameplayAsyncLoadedDelegate.BindUObject(this, &UG2ISavingGameplayManager::OnGameplayAsyncLoaded);
}

void UG2ISavingGameplayManager::CreateNewGameplaySaveGameObject()
{
    if (UGameplayStatics::DoesSaveGameExist(GameplaySaveSlotName, 0))
        UGameplayStatics::DeleteGameInSlot(GameplaySaveSlotName, 0);

    CreateGameplaySaveGame();
}

bool UG2ISavingGameplayManager::CreateGameplaySaveGame()
{
    GameplaySaveGame = Cast<UG2IGameplaySaveGame>(UGameplayStatics::CreateSaveGameObject(UG2IGameplaySaveGame::StaticClass()));
    if (!ensure(GameplaySaveGame))
    {
        UE_LOG(LogG2I, Error, TEXT("Couldn't create GameplaySaveGame object in %s."), *GetName());
        return false;
    }

    return true;
}

void UG2ISavingGameplayManager::OnGameplayAsyncSaved(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
    OnGameplaySavedDelegate.Broadcast(bSuccess);
    if (bSuccess)
    {
        UE_LOG(LogG2I, Log, TEXT("Gameplay saved successfully in the slot %s."), *GameplaySaveSlotName);
    }
    else
        UE_LOG(LogG2I, Error, TEXT("Gameplay saving in the slot %s failed."), *GameplaySaveSlotName);
}

void UG2ISavingGameplayManager::OnGameplayAsyncLoaded(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
    if (!LoadedGameData)
    {
        // Load failed
        UE_LOG(LogG2I, Error, TEXT("Gameplay load from the slot %s failed."), *GameplaySaveSlotName);
        OnGameplayLoadedDelegate.Broadcast(false);
        return;
    }

    GameplaySaveGame = Cast<UG2IGameplaySaveGame>(LoadedGameData);
    if (GameplaySaveGame)
    {
        UE_LOG(LogG2I, Log, TEXT("Gameplay loaded successfully from the slot %s."), *GameplaySaveSlotName);
        OnGameplayLoadedDelegate.Broadcast(true);
    }
    else
    {
        // SaveGame file exists, but not valid
        UE_LOG(LogG2I, Error, TEXT("Gameplay loaded from the slot %s but is invalid. Operation failed."), *GameplaySaveSlotName);
        OnGameplayLoadedDelegate.Broadcast(false);
    }
}

void UG2ISavingGameplayManager::SaveGameplay(const bool bAsync)
{
    if (!GameplaySaveGame)
    {
        if (!CreateGameplaySaveGame())
        {
            return;
        }
    }

    OnGameplaySaveStartedDelegate.Broadcast();

    if (bAsync)
    {
        // Asynchronous
        UGameplayStatics::AsyncSaveGameToSlot(GameplaySaveGame, GameplaySaveSlotName, 0, OnGameplayAsyncSavedDelegate);
    }
    else
    {
        // Synchronous
        if (UGameplayStatics::SaveGameToSlot(GameplaySaveGame, GameplaySaveSlotName, 0))
        {
            // Successfully saved
            OnGameplaySavedDelegate.Broadcast(true);
            UE_LOG(LogG2I, Log, TEXT("Gameplay saved successfully in the slot %s."), *GameplaySaveSlotName);
        }
        else
        {
            // Failed to save
            OnGameplaySavedDelegate.Broadcast(false);
            UE_LOG(LogG2I, Error, TEXT("Gameplay saving in the slot %s failed."), *GameplaySaveSlotName);
        }
    }
}

void UG2ISavingGameplayManager::SetGameplaySaveSlotName(const FString& NewSlotName)
{
    GameplaySaveSlotName = NewSlotName;
}

const FString& UG2ISavingGameplayManager::GetGameplaySaveSlotName() const
{
    return GameplaySaveSlotName;
}

void UG2ISavingGameplayManager::LoadGameplay(const bool bAsync)
{
    OnGameplayLoadStartedDelegate.Broadcast();
    if (bAsync)
    {
        // Asynchronous
        UGameplayStatics::AsyncLoadGameFromSlot(GameplaySaveSlotName, 0, OnGameplayAsyncLoadedDelegate);
    }
    else
    {
        // Synchronous
        if (USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(GameplaySaveSlotName, 0))
        {
            if (GameplaySaveGame = Cast<UG2IGameplaySaveGame>(LoadedGame))
            {
                // Successfully loaded
                OnGameplayLoadedDelegate.Broadcast(true);
                UE_LOG(LogG2I, Log, TEXT("Gameplay loaded successfully from the slot %s."), *GameplaySaveSlotName);
            }
            else
            {
                // Loaded SaveGame object, but it's invalid
                OnGameplayLoadedDelegate.Broadcast(false);
                UE_LOG(LogG2I, Error, TEXT("Gameplay loaded from the slot %s but is invalid. Operation failed."), *GameplaySaveSlotName);
            }
        }
        else
        {
            // Failed to load
            OnGameplayLoadedDelegate.Broadcast(false);
            UE_LOG(LogG2I, Error, TEXT("Gameplay load from the slot %s failed."), *GameplaySaveSlotName);
        }
    }
}

void UG2ISavingGameplayManager::SaveAllData()
{
    if (!ensure(GameplaySaveGame))
    {
        UE_LOG(LogG2I, Error, TEXT("GameplaySaveGame in null in %s."), *GetName());
        return;
    }

    // Getting all actors with interface 'Savable'
    TArray<AActor*> FoundSavableActors;
    UGameplayStatics::GetAllActorsWithInterface(
        this,
        UG2ISavableInterface::StaticClass(),
        FoundSavableActors);


    // Iterating on them & saving their data
    for (auto* Actor : FoundSavableActors)
    {
        IG2ISavableInterface::Execute_SaveData(Actor, GameplaySaveGame);
    }

    UE_LOG(LogG2I, Log, TEXT("Gameplay data saved & stored in GameplaySaveGame object."));
}

void UG2ISavingGameplayManager::LoadAllData() const
{
    if (!ensure(GameplaySaveGame))
    {
        UE_LOG(LogG2I, Error, TEXT("GameplaySaveGame in null in %s."), *GetName());
        return;
    }

    // Getting all actors with interface 'Savable'
    TArray<AActor*> FoundSavableActors;
    UGameplayStatics::GetAllActorsWithInterface(
        this,
        UG2ISavableInterface::StaticClass(),
        FoundSavableActors);

    // Iterating on them & loading their data
    for (auto* Actor : FoundSavableActors)
    {
        IG2ISavableInterface::Execute_LoadData(Actor, GameplaySaveGame);
    }

    UE_LOG(LogG2I, Log, TEXT("Gameplay data loaded from the GameplaySaveGame object."));
}

void UG2ISavingGameplayManager::SaveAllDataAndGameplay(const bool bAsync)
{
    if (!GameplaySaveGame)
    {
        if (!CreateGameplaySaveGame())
        {
            return;
        }
    }

    SaveAllData();
    SaveGameplay(bAsync);
}

void UG2ISavingGameplayManager::SaveRequestedData(UObject* Requester)
{
    if (!ensure(GameplaySaveGame))
    {
        UE_LOG(LogG2I, Error, TEXT("GameplaySaveGame in null. You'll have to load it first or create new."));
        return;
    }

    if (Requester)
    {
        if (Requester->Implements<UG2ISavableInterface>())
        {
            IG2ISavableInterface::Execute_SaveData(Requester, GameplaySaveGame);
        }
        else
            UE_LOG(LogG2I, Warning, TEXT("%s doesn't implement Savable interface. It's data will be lost."), *Requester->GetName());
    }
}

void UG2ISavingGameplayManager::LoadRequestedData(UObject* Requester) const
{
    if (!ensure(GameplaySaveGame))
    {
        UE_LOG(LogG2I, Error, TEXT("GameplaySaveGame in null. You'll have to load it first or create new."));
        return;
    }

    if (Requester)
    {
        if (Requester->Implements<UG2ISavableInterface>())
        {
            IG2ISavableInterface::Execute_LoadData(Requester, GameplaySaveGame);
        }
        else
            UE_LOG(LogG2I, Warning, TEXT("%s doesn't implement Savable interface. It's data won't be loaded."), *Requester->GetName());
    }
}

bool UG2ISavingGameplayManager::DoesSaveExist() const
{
    return UGameplayStatics::DoesSaveGameExist(GameplaySaveSlotName, 0);
}

void UG2ISavingGameplayManager::SaveCurrentLevel(EG2ILevelName LevelEnum, bool bAsync)
{
    if (!GameplaySaveGame)
    {
        if (!CreateGameplaySaveGame())
        {
            return;
        }

    }
    GameplaySaveGame->CurrentLevel = LevelEnum;
    SaveGameplay(bAsync);
}

EG2ILevelName UG2ISavingGameplayManager::LoadCurrentLevel() const
{
    if (!GameplaySaveGame)
    {
        return EG2ILevelName::None;
    }
    return GameplaySaveGame->CurrentLevel;
}

void UG2ISavingGameplayManager::ResetProgress()
{
    CreateNewGameplaySaveGameObject();
    SaveGameplay(false);
}