#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SaveGame.h"
#include "Interfaces/SavingSystem/G2ISaveGameplayInterface.h"
#include "Interfaces/SavingSystem/G2ISaveSettingsInterface.h"
#include "Game/G2IGameplaySaveGame.h"
#include "Game/G2ISettingsSaveGame.h"
#include "G2IGameInstance.generated.h"

class UG2IWidgetsCatalog;

/**
 * Base game instance for G2I game
 * saves and loads gameplay & settings data
 */
UCLASS()
class G2I_API UG2IGameInstance : public UGameInstance, public IG2ISaveGameplayInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2IWidgetsCatalog> WidgetsCatalog;

	// Class that holds SaveGameplay-related delegates
	UPROPERTY(BlueprintReadOnly, Category = "Gameplay Save Game")
	TObjectPtr<UG2ISaveGameplayDelegates> SaveGameplayDelegates;

private:
	// SaveGame w/ gameplay info
	UPROPERTY()
	TObjectPtr<UG2IGameplaySaveGame> GameplaySaveGame;

	// SaveGame w/ settings
	UPROPERTY()
	TObjectPtr<UG2ISettingsSaveGame> SettingsSaveGame;

	// Name of the current slot for the gameplay save game
	UPROPERTY()
	FString GameplaySaveSlotName = TEXT("GameplaySaveSlot0");

	// Name of the current slot for the gameplay save game
	UPROPERTY()
	FString SettingsSaveSlotName = TEXT("SettingsSaveSlot0");

	// C++-only delegate called from AsyncSaveGameToSlot
	FAsyncSaveGameToSlotDelegate OnGameplayAsyncSavedDelegate;

	// C++-only delegate called from AsyncLoadGameToSlot
	FAsyncLoadGameFromSlotDelegate OnGameplayAsyncLoadedDelegate;

public:

	// === Implementations of the IG2ISaveGameplayInterface' functions ===

	void CreateNewGameplaySaveGameObject_Implementation();

	void SaveGameplay_Implementation(bool bAsync);

	void LoadGameplay_Implementation(bool bAsync);

	void SetGameplaySaveSlotName_Implementation(const FString& NewSlotName);

	const FString GetGameplaySaveSlotName_Implementation();

	void SaveAllData_Implementation();

	void LoadAllData_Implementation();

	void SaveAllDataAndGameplay_Implementation(bool bAsync);

	void SaveRequestedData_Implementation(UObject* Requester);

	void LoadRequestedData_Implementation(UObject* Requester);

	const UG2ISaveGameplayDelegates* GetGameplaySaveDelegates_Implementation();

	// === Other Functions ===

protected:
	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category = "Gameplay Save Game")
	void CreateGameplaySaveGame();

	UFUNCTION(BlueprintCallable, Category = "Gameplay Save Game")
	void CreateSaveGameplayDelegates();

	UFUNCTION(BlueprintCallable, Category = "Gameplay Save Game")
	void SyncGameplayLoadGameData();

	// Finding all actors that have gameplay tag 'TagName'
	UFUNCTION(BlueprintCallable)
	void GetAllActorsWithGameplayTag(TArray<AActor*>& FoundActors, FName TagName);

	void OnGameplayAsuncSaved(const FString& SlotName, const int32 UserIndex, bool bSuccess);

	void OnGameplayAsuncLoaded(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);
};
