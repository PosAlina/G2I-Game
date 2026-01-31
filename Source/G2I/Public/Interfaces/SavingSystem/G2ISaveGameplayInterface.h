

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Delegates/Delegate.h"
#include "G2ISaveGameplayInterface.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameplaySavedSignature, bool /* bSuccess */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameplayLoadedSignature, bool /* bSuccess */);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UG2ISaveGameplayInterface : public UInterface
{
	GENERATED_BODY()
};

/*
 Allows to save, load, sync data with SaveGame object that corresponds to storing gameplay data
 (such as player's location, inventory, etc.) and to return delegates of that actions.
 */
class G2I_API IG2ISaveGameplayInterface
{
	GENERATED_BODY()

public:
	// Creates save game object
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Save Gameplay")
	void CreateGameplaySaveGameObject();

	// Saves game
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Save Gameplay")
	void SaveGameplay(bool bAsync);

	// Loads game
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Save Gameplay")
	void LoadGameplay(bool bAsync);

	// Syncs game data
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Save Gameplay")
	void SyncGameplaySaveGameData();

	// Syncs data & saves game
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Save Gameplay")
	void SuncAndSaveGameplay(bool bAsync);

	// Returns OnGameplaySaved Delegate. Not Blueprint compatible.
	FOnGameplaySavedSignature GetGameplaySavedDelegate();

	// Returns OnGameplayLoaded Delegate. Not Blueprint compatible.
	FOnGameplayLoadedSignature GetGameplayLoadedDelegate();

};
