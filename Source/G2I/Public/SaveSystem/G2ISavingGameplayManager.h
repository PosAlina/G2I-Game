#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Delegates/Delegate.h"
#include "Kismet/GameplayStatics.h"
#include "G2IGameplaySaveGame.h"
#include "G2ISavingGameplayManager.generated.h"

// Delegates

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameplaySavedDelegate, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameplaySaveStartedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameplayLoadedDelegate, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameplayLoadStartedDelegate);

/**
 * Responsible for saving, loading & storing gameplay data
 */
UCLASS()
class G2I_API UG2ISavingGameplayManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintAssignable)
	FOnGameplaySavedDelegate OnGameplaySavedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnGameplaySaveStartedDelegate OnGameplaySaveStartedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnGameplayLoadedDelegate OnGameplayLoadedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnGameplayLoadStartedDelegate OnGameplayLoadStartedDelegate;

protected:

	UPROPERTY()
	FString GameplaySaveSlotName = TEXT("GameplaySaveSlot0");

	// C++-only delegate called from AsyncSaveGameToSlot
	FAsyncSaveGameToSlotDelegate OnGameplayAsyncSavedDelegate;

	// C++-only delegate called from AsyncLoadGameToSlot
	FAsyncLoadGameFromSlotDelegate OnGameplayAsyncLoadedDelegate;

private:
	// SaveGame with gameplay info
	UPROPERTY()
	TObjectPtr<UG2IGameplaySaveGame> GameplaySaveGame;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	void CreateNewGameplaySaveGameObject();

	UFUNCTION(BlueprintCallable)
	void SetGameplaySaveSlotName(const FString& NewSlotName);

	UFUNCTION(BlueprintCallable)
	const FString& GetGameplaySaveSlotName() const;

	// Saves SaveGame to slot
	UFUNCTION(BlueprintCallable)
	void SaveGameplay(const bool bAsync);

	// Loads SaveGame from slot
	UFUNCTION(BlueprintCallable)
	void LoadGameplay(const bool bAsync);

	// Saves data of all ACTORS on the level with Savable interface in the SaveGame (not in the slot!)
	UFUNCTION(BlueprintCallable)
	void SaveAllData();

	// Loads data of all ACTORS on the level with Savable interface from the SaveGame
	UFUNCTION(BlueprintCallable)
	void LoadAllData() const;

	// Saves data of all ACTORS on the level with Savable interface in the SaveGame & saves SaveGame to slot
	UFUNCTION(BlueprintCallable)
	void SaveAllDataAndGameplay(const bool bAsync);

	// Saves Requester's data in the SaveGame object (not in the slot!)
	// This will be called when Requester needs it (in it's OnDestroyed, etc.)
	UFUNCTION(BlueprintCallable)
	void SaveRequestedData(UObject* Requester);

	// Loads Requester's data from the SaveGame object (not from the slot!)
	// This will be called when Requester needs it (in it's BeginPlay, etc.)
	UFUNCTION(BlueprintCallable)
	void LoadRequestedData(UObject* Requester) const;

	UFUNCTION(BlueprintCallable)
	bool DoesSaveExist() const;

	UFUNCTION(BlueprintCallable)
	void SaveCurrentLevel(EG2ILevelName LevelEnum, bool bAsync = false);

	UFUNCTION(BlueprintCallable)
	EG2ILevelName LoadCurrentLevel() const;

	UFUNCTION(BlueprintCallable)
	void ResetProgress();

protected:

	UFUNCTION(BlueprintCallable)
	bool CreateGameplaySaveGame();

	void OnGameplayAsyncSaved(const FString& SlotName, const int32 UserIndex, bool bSuccess);

	void OnGameplayAsyncLoaded(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);
};
