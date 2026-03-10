#include "G2IGameInstance.h"
#include "G2I.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "BlueprintGameplayTagLibrary.h"
#include "Interfaces/SavingSystem/G2ISavableInterface.h"

void UG2IGameInstance::CreateNewGameplaySaveGameObject_Implementation()
{
    if (UGameplayStatics::DoesSaveGameExist(GameplaySaveSlotName, 0))
        UGameplayStatics::DeleteGameInSlot(GameplaySaveSlotName, 0);

    CreateGameplaySaveGame();
}

void UG2IGameInstance::Init()
{
    Super::Init();

    CreateSaveGameplayDelegates();
    CreateGameplaySaveGame();
}

void UG2IGameInstance::CreateGameplaySaveGame()
{
    GameplaySaveGame = Cast<UG2IGameplaySaveGame>(UGameplayStatics::CreateSaveGameObject(UG2IGameplaySaveGame::StaticClass()));
    if (!ensure(GameplaySaveGame))
        UE_LOG(LogG2I, Error, TEXT("Couldn't create GameplaySaveGame object in %s."), *GetName());
}

void UG2IGameInstance::CreateSaveGameplayDelegates()
{
    SaveGameplayDelegates = NewObject<UG2ISaveGameplayDelegates>(this);
    if (!ensure(SaveGameplayDelegates))
    {
        UE_LOG(LogG2I, Error, TEXT("Couldn't create SaveGameplayDelegates object in %s."), *GetName());
        return;
    }

    OnGameplayAsyncSavedDelegate.BindUObject(this, &UG2IGameInstance::OnGameplayAsuncSaved);
    OnGameplayAsyncLoadedDelegate.BindUObject(this, &UG2IGameInstance::OnGameplayAsuncLoaded);
}

void UG2IGameInstance::OnGameplayAsuncSaved(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
    SaveGameplayDelegates->OnGameplaySavedDelegate.Broadcast(bSuccess);
    if (bSuccess)
    {
        UE_LOG(LogG2I, Log, TEXT("Gameplay saved successfully in the slot %s."), *GameplaySaveSlotName);
    }
    else
        UE_LOG(LogG2I, Error, TEXT("Gameplay saving in the slot %s failed."), *GameplaySaveSlotName);
}

void UG2IGameInstance::OnGameplayAsuncLoaded(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
    if (LoadedGameData)
    {
        if (GameplaySaveGame = Cast<UG2IGameplaySaveGame>(LoadedGameData))
        {
            UE_LOG(LogG2I, Log, TEXT("Gameplay loaded successfully from the slot %s."), *GameplaySaveSlotName);
            SaveGameplayDelegates->OnGameplayLoadedDelegate.Broadcast(true);
        }
        else
        {
            // SaveGame file exists, but not valid
            UE_LOG(LogG2I, Error, TEXT("Gameplay loaded from the slot %s but is invalid. Operation failed."), *GameplaySaveSlotName);
            SaveGameplayDelegates->OnGameplayLoadedDelegate.Broadcast(false);
        }
    }
    else
    {
        // Load failed
        UE_LOG(LogG2I, Error, TEXT("Gameplay load from the slot %s failed."), *GameplaySaveSlotName);
        SaveGameplayDelegates->OnGameplayLoadedDelegate.Broadcast(false);
    }
}

void UG2IGameInstance::SaveGameplay_Implementation(bool bAsync)
{
    if (!GameplaySaveGame)
        CreateGameplaySaveGame();
    if (!SaveGameplayDelegates)
        CreateSaveGameplayDelegates();

    if (bAsync)
    {
        // Asynchronous
        SaveGameplayDelegates->OnGameplaySaveStartedDelegate.Broadcast();
        UGameplayStatics::AsyncSaveGameToSlot(GameplaySaveGame, GameplaySaveSlotName, 0, OnGameplayAsyncSavedDelegate);
    }
    else
    {
        // Synchronous
        SaveGameplayDelegates->OnGameplaySaveStartedDelegate.Broadcast();
        if (UGameplayStatics::SaveGameToSlot(GameplaySaveGame, GameplaySaveSlotName, 0))
        {
            // Successfully saved
            SaveGameplayDelegates->OnGameplaySavedDelegate.Broadcast(true);
            UE_LOG(LogG2I, Log, TEXT("Gameplay saved successfully in the slot %s."), *GameplaySaveSlotName);
        }
        else
        {
            // Failed to save
            SaveGameplayDelegates->OnGameplaySavedDelegate.Broadcast(false);
            UE_LOG(LogG2I, Error, TEXT("Gameplay saving in the slot %s failed."), *GameplaySaveSlotName);
        }
    }
}

void UG2IGameInstance::SetGameplaySaveSlotName_Implementation(const FString& NewSlotName)
{
	GameplaySaveSlotName = NewSlotName;
}

const FString UG2IGameInstance::GetGameplaySaveSlotName_Implementation()
{
	return GameplaySaveSlotName;
}

UG2ISaveGameplayDelegates* UG2IGameInstance::GetGameplaySaveDelegates_Implementation()
{
    return SaveGameplayDelegates;
}

void UG2IGameInstance::LoadGameplay_Implementation(bool bAsync)
{
    if (!SaveGameplayDelegates)
        CreateSaveGameplayDelegates();

    if (bAsync)
    {
        // Asynchronous
        SaveGameplayDelegates->OnGameplayLoadStartedDelegate.Broadcast();
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
                UE_LOG(LogG2I, Log, TEXT("Gameplay loaded successfully from the slot %s."), *GameplaySaveSlotName);
            }
            else
            {
                // Loaded SaveGame object, but it's invalid
                UE_LOG(LogG2I, Error, TEXT("Gameplay loaded from the slot %s but is invalid. Operation failed."), *GameplaySaveSlotName);
            }
        }
        else
        {
            // Failed to load
            UE_LOG(LogG2I, Error, TEXT("Gameplay load from the slot %s failed."), *GameplaySaveSlotName);
        }
    }
}

void UG2IGameInstance::SaveAllData_Implementation()
{
    if (!GameplaySaveGame)
    {
        UE_LOG(LogG2I, Error, TEXT("GameplaySaveGame in null. You'll have to load it first or create new."));
        return;
    }

    // Getting all actors with interface 'Savable'
    TArray<AActor*> FoundSavableActors;
    GetAllActorsWithSavableIntetrface(FoundSavableActors);

    // Iterating on them & saving their data
    for (auto* Actor : FoundSavableActors)
    {
        IG2ISavableInterface::Execute_SaveData(Actor, GameplaySaveGame);
    }

    UE_LOG(LogG2I, Log, TEXT("Gameplay data saved & stored in GameplaySaveGame object."));
}

void UG2IGameInstance::LoadAllData_Implementation()
{
    if (!GameplaySaveGame)
    {
        UE_LOG(LogG2I, Error, TEXT("GameplaySaveGame in null. You'll have to load it first or create new."));
        return;
    }

    // Getting all actors with interface 'Savable'
    TArray<AActor*> FoundSavableActors;
    GetAllActorsWithSavableIntetrface(FoundSavableActors);

    // Iterating on them & loading their data
    for (auto* Actor : FoundSavableActors)
    {
        IG2ISavableInterface::Execute_LoadData(Actor, GameplaySaveGame);
    }

    UE_LOG(LogG2I, Log, TEXT("Gameplay data loaded from the GameplaySaveGame object."));
}

void UG2IGameInstance::GetAllActorsWithSavableIntetrface(TArray<AActor*>& FoundActors)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s."), *GetName());
        return;
    }

    UGameplayStatics::UGameplayStatics::GetAllActorsWithInterface(
        this,
        UG2ISavableInterface::StaticClass(),
        FoundActors);
}

void UG2IGameInstance::SaveAllDataAndGameplay_Implementation(bool bAsync)
{
    IG2ISaveGameplayInterface::Execute_SaveAllData(this);
    IG2ISaveGameplayInterface::Execute_SaveGameplay(this, bAsync);
}

void UG2IGameInstance::SaveRequestedData_Implementation(UObject* Requester)
{
    if (!GameplaySaveGame)
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

void UG2IGameInstance::LoadRequestedData_Implementation(UObject* Requester)
{
    if (!GameplaySaveGame)
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
