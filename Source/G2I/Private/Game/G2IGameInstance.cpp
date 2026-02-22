#include "G2IGameInstance.h"
#include "G2I.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayTagContainer.h"
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

    OnGameplayAsyncSavedDelegate.BindUObject(this, &UG2IGameInstance::OnGameplayAsuncSaved);
    OnGameplayAsyncLoadedDelegate.BindUObject(this, &UG2IGameInstance::OnGameplayAsuncLoaded);

    /*if (UGameplayStatics::DoesSaveGameExist(GameplaySaveSlotName, 0))
    {
        // Load?
        // Check if it's valid?
        // idk?
    }
    else
    {
        CreateGameplaySaveGame();
    }*/
}

void UG2IGameInstance::CreateGameplaySaveGame()
{
    GameplaySaveGame = Cast<UG2IGameplaySaveGame>(UGameplayStatics::CreateSaveGameObject(UG2IGameplaySaveGame::StaticClass()));
    if (!ensure(GameplaySaveGame))
        UE_LOG(LogG2I, Error, TEXT("Couldn't create GameplaySaveGame object."));
}

void UG2IGameInstance::CreateSaveGameplayDelegates()
{
    SaveGameplayDelegates = NewObject<UG2ISaveGameplayDelegates>(this);
    if (!ensure(SaveGameplayDelegates))
        UE_LOG(LogG2I, Error, TEXT("Couldn't create SaveGameplayDelegates object."));
}

void UG2IGameInstance::OnGameplayAsuncSaved(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
    SaveGameplayDelegates->OnGameplaySavedDelegate.Broadcast(bSuccess);
}

void UG2IGameInstance::OnGameplayAsuncLoaded(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
    if (LoadedGameData)
    {
        if (GameplaySaveGame = Cast<UG2IGameplaySaveGame>(LoadedGameData))
        {
            // Successfully loaded
        }
        else
        {
            // SaveGame file exists, but not valid
            // TODO
        }

        SaveGameplayDelegates->OnGameplayLoadedDelegate.Broadcast(true);
        SyncGameplayLoadGameData();
    }
    else
    {
        // Load failed
        SaveGameplayDelegates->OnGameplayLoadedDelegate.Broadcast(false);
    }
}

void UG2IGameInstance::SaveGameplay_Implementation(bool bAsync)
{
    if (!ensure(GameplaySaveGame))
        CreateGameplaySaveGame();
    if (!ensure(SaveGameplayDelegates))
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
        if (ensure(UGameplayStatics::SaveGameToSlot(GameplaySaveGame, GameplaySaveSlotName, 0)))
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

const UG2ISaveGameplayDelegates* UG2IGameInstance::GetGameplaySaveDelegates_Implementation()
{
    return SaveGameplayDelegates;
}

void UG2IGameInstance::LoadGameplay_Implementation(bool bAsync)
{
    if (!ensure(SaveGameplayDelegates))
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
        SaveGameplayDelegates->OnGameplayLoadStartedDelegate.Broadcast();
        if (USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(GameplaySaveSlotName, 0))
        {
            if (GameplaySaveGame = Cast<UG2IGameplaySaveGame>(LoadedGame))
            {
                // Successfully loaded
                SaveGameplayDelegates->OnGameplayLoadedDelegate.Broadcast(true);
                SyncGameplayLoadGameData();
                UE_LOG(LogG2I, Log, TEXT("Gameplay saved successfully in the slot %s."), *GameplaySaveSlotName);
            }
            else
            {
                // Loaded SaveGame object, but it's invalid
                // TODO
            }
        }
        else
        {
            // Failed to load
            SaveGameplayDelegates->OnGameplayLoadedDelegate.Broadcast(false);
            UE_LOG(LogG2I, Error, TEXT("Gameplay saving in the slot %s failed."), *GameplaySaveSlotName);
            
            // TODO maybe create new SaveGame object?
        }
    }
}

void UG2IGameInstance::SyncGameplaySaveGameData_Implementation()
{
    // Getting all actors with tag 'Savable'
    TArray<AActor*> FoundSavableActors;
    GetAllActorsWithGameplayTag(FoundSavableActors, FName(TEXT("Savable")));

    // Iterating on them & saving their data
    for (auto* Actor : FoundSavableActors)
    {
        if (Actor->Implements<IG2ISavableInterface>())
        {
            IG2ISavableInterface::Execute_SaveData(Actor, GameplaySaveGame);
        }
        else
            UE_LOG(LogG2I, Error, TEXT("Actor %s doesn't implement Savable interface. It's data will be lost."), *Actor->GetActorNameOrLabel());
    }

    UE_LOG(LogG2I, Log, TEXT("Gameplay data synced & stored in GameplaySaveGame object."));
}

void UG2IGameInstance::SyncGameplayLoadGameData()
{
    // Getting all actors with tag 'Savable'
    TArray<AActor*> FoundSavableActors;
    GetAllActorsWithGameplayTag(FoundSavableActors, FName(TEXT("Savable")));

    // Iterating on them & loading their data
    for (auto* Actor : FoundSavableActors)
    {
        if (Actor->Implements<IG2ISavableInterface>())
        {
            IG2ISavableInterface::Execute_LoadData(Actor, GameplaySaveGame);
        }
        else
            UE_LOG(LogG2I, Error, TEXT("Actor %s doesn't implement Savable interface. It's data will be lost."), *Actor->GetActorNameOrLabel());
    }

    UE_LOG(LogG2I, Log, TEXT("Gameplay data synced & loaded in Savable objects."));
}

void UG2IGameInstance::GetAllActorsWithGameplayTag(TArray<AActor*>& FoundActors, FName TagName)
{
    if (!ensure(GameplaySaveGame))
        CreateGameplaySaveGame();

    UWorld* World = GetWorld();
    if (!World)
        return;

    TArray<AActor*> FoundSavableActors;
    FGameplayTagQuery TagQuery = FGameplayTagQuery::MakeQuery_MatchTag(FGameplayTag::RequestGameplayTag(TagName));

    UBlueprintGameplayTagLibrary::GetAllActorsOfClassMatchingTagQuery(
        World,
        AActor::StaticClass(),
        TagQuery,
        FoundActors
    );
}

void UG2IGameInstance::SuncAndSaveGameplay_Implementation(bool bAsync)
{
    IG2ISaveGameplayInterface::Execute_SyncGameplaySaveData(this);
    IG2ISaveGameplayInterface::Execute_SaveGameplay(this, bAsync);
}

void UG2IGameInstance::CheckIfGameplaySaveGameExistsAndValid()
{

}

void UG2IGameInstance::CheckIfSettingsSaveGameExistsAndValid()
{

}