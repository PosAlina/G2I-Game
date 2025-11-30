#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "G2IPlayerState.generated.h"

class UDataTable;
class AAIController;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewControllerPossessDelegat, APawn*, Pawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewControllerPossessDelegat, APawn*, Pawn);

/**
 * Player state contains info about playable characters
 */
UCLASS()
class G2I_API AG2IPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = Characters)
	uint32 NumberCurrentCharacter = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Characters)
	TObjectPtr<UDataTable> PlayableCharactersDataTable;

	TArray<FName> PlayableCharactersRowNames;

	UPROPERTY(BlueprintAssignable)
	FNewControllerPossessDelegat OnNewControllerPossessDelegate;

public:
	
	virtual void BeginPlay() override;

	void SelectNextCharacter();

protected:

	APawn *GetPawn(const uint32 PawnNumber);
	
	void SetupPlayableDataTable();
	
	void SetupPlayableCharacters();

	bool SetupPlayerControllerForPawn(const uint32 PawnNumber, APawn& CurrentPawn);
	
	bool SetupControllerForPawn(const uint32 PawnNumber);
	
	bool SetupControllerForPawn(const uint32 PawnNumber, const TSubclassOf<AActor> AIControllerActorClass,
    	APawn& CurrentPawn);
    	
    bool SetupControllerForPawn(const uint32 PawnNumber, AAIController& CurrentAIController,
        	APawn& CurrentPawn);
	
	AActor *FindOneActor(const TSubclassOf<AActor> ActorClass) const;
	
	AActor *SpawnActor(const TSubclassOf<AActor> ActorClass) const;

};
