#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "G2IPlayerState.generated.h"

class UDataTable;

/**
 * Player state contains info about playable characters
 */
UCLASS()
class G2I_API AG2IPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	void SetupPlayableDataTable();

	void SetupDefaultCharacter();

	UPROPERTY(EditAnywhere, Category = Characters)
	uint32 NumberCurrentCharacter = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Characters)
	TObjectPtr<UDataTable> PlayableCharactersDataTable;

	TArray<FName> PlayableCharactersRowNames;
	
	void SpawnCharacter(const uint32 NewCharacterNumber, TSubclassOf<AActor> CharacterClass,
		const FVector& CharacterSpawnLocation, const FRotator& CharacterSpawnRotation);

	void SelectCharacter(const uint32 NewCharacterNumber,APawn &NewCharacter);

	void SelectNextCharacter(const FVector& CharacterSpawnLocation, const FRotator& CharacterSpawnRotation);
	
	void SwitchCharacters(uint32 NewCharacterNumber, const FVector& CharacterSpawnLocation, const FRotator& CharacterSpawnRotation);
};
