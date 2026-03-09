#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "G2IGameplaySaveGame.generated.h"

USTRUCT(BlueprintType)
struct FPlayersSaveData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, BlueprintReadWrite)
	TMap<FString, FVector> CharactersLocation;

	UPROPERTY(SaveGame, BlueprintReadWrite)
	FString CurrentCharacter = TEXT("");
};

/**
 * SaveGame for gameplay data.
 */
UCLASS()
class G2I_API UG2IGameplaySaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save Gameplay Data|Player")
	FPlayersSaveData PlayersSaveData;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save Gameplay Data|Saving Trigger Boxes")
	TMap<FString, bool> SaveTriggerBoxesSaveData;
};
