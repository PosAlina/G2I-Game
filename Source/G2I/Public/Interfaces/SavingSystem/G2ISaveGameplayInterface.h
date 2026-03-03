#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
//#include "Kismet/GameplayStatics.h"
#include "Delegates/Delegate.h"
#include "G2ISaveGameplayInterface.generated.h"


// === Delegates ===

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameplaySavedDelegate, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameplaySaveStartedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameplayLoadedDelegate, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameplayLoadStartedDelegate);

// Wrapper class for save gameplay delegates so that we can return it in the interface function
UCLASS(Blueprintable)
class G2I_API UG2ISaveGameplayDelegates : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Save Gameplay Delegates")
	FOnGameplaySavedDelegate OnGameplaySavedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Save Gameplay Delegates")
	FOnGameplaySaveStartedDelegate OnGameplaySaveStartedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Save Gameplay Delegates")
	FOnGameplayLoadedDelegate OnGameplayLoadedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Save Gameplay Delegates")
	FOnGameplayLoadStartedDelegate OnGameplayLoadStartedDelegate;
};



// === Interface ===


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
	// Creates new save game object, sets everything as default
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Save Gameplay")
	void CreateNewGameplaySaveGameObject();

	// Sets slot name
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Save Gameplay")
	void SetGameplaySaveSlotName(const FString& NewSlotName);

	// Returns slot name
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Save Gameplay")
	const FString GetGameplaySaveSlotName();

	// Saves game
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Save Gameplay")
	void SaveGameplay(bool bAsync);

	// Loads game & syncs loaded data with every object that has Savable interface
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Save Gameplay")
	void LoadGameplay(bool bAsync);

	// Updates & syncs data of every object with the Savable interface
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Save Gameplay")
	void SyncGameplaySaveData();

	// Syncs all data & saves game
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Save Gameplay")
	void SuncAndSaveGameplay(bool bAsync);

	// Returns class with all delegates
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Save Gameplay")
	const UG2ISaveGameplayDelegates* GetGameplaySaveDelegates();
};
