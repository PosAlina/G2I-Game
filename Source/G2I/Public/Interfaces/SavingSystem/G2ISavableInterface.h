#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Game/G2IGameplaySaveGame.h"
#include "G2ISavableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UG2ISavableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface that every object that needs to be saved have to implement
 */
class G2I_API IG2ISavableInterface
{
	GENERATED_BODY()

public:
	// Adds/updates needed data to the save game object
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Savable")
	void SaveData(UG2IGameplaySaveGame* SaveGameRef);

	// Loads needed data from the save game object
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Savable")
	void LoadData(const UG2IGameplaySaveGame* SaveGameRef);
};
