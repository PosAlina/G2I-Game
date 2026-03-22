#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameFramework/Character.h"
#include "Misc/Crc.h"
#include "G2IGameplaySaveGame.generated.h"

USTRUCT(BlueprintType)
struct FPlayerSaveData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, BlueprintReadWrite)
	TMap<TSubclassOf<ACharacter>, FTransform> CharactersTransform;

	UPROPERTY(SaveGame, BlueprintReadWrite)
	TSubclassOf<ACharacter> CurrentCharacter;
};

USTRUCT(BlueprintType)
struct FCheckpointSaveData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, BlueprintReadWrite)
	FString LevelName;

	UPROPERTY(SaveGame, BlueprintReadWrite)
	FVector Location;

	// Extending the struct for using it in TMap as a key

	FCheckpointSaveData(const FString& NewLevelName, const FVector& NewLocation)
		: LevelName(NewLevelName), Location(NewLocation) {
	}

	FCheckpointSaveData()
		: LevelName(TEXT("")), Location(FVector::ZeroVector) {
	}

	FCheckpointSaveData(const FCheckpointSaveData& Other)
		: LevelName(Other.LevelName), Location(Other.Location) {
	}

	bool operator==(const FCheckpointSaveData& Other) const
	{
		return Equals(Other);
	}

	bool operator!=(const FCheckpointSaveData& Other) const
	{
		return !Equals(Other);
	}

	bool Equals(const FCheckpointSaveData& Other) const
	{
		return LevelName.Equals(Other.LevelName) && Location.Equals(Other.Location);
	}

	friend uint32 GetTypeHash(const FCheckpointSaveData& Thing)
	{
		const uint32 Hash = FCrc::MemCrc32(&Thing, sizeof(FCheckpointSaveData));
		return Hash;
	}
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
	FPlayerSaveData PlayersSaveData;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save Gameplay Data|Checkpoints")
	TMap<FCheckpointSaveData, bool> CheckpointsSaveData;
};
