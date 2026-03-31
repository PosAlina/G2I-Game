#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "G2ISavableInterface.h"
#include "G2ISavingGameplayManager.h"
#include "G2ICheckpoint.generated.h"

/**
 * Trigger box that, when triggered by the player, saves gameplay & deletes itself
 */
UCLASS()
class G2I_API AG2ICheckpoint : public ATriggerBox, public IG2ISavableInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UG2ISavingGameplayManager> SavingGameplayManager;

	UPROPERTY()
	bool bActivated = false;

public:
	AG2ICheckpoint();

	UFUNCTION()
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION()
	void OnGameplaySaved(bool bSuccess);

	virtual void SaveData_Implementation(UG2IGameplaySaveGame* SaveGameRef) override;

	virtual void LoadData_Implementation(const UG2IGameplaySaveGame* SaveGameRef) override;

protected:
	virtual void BeginPlay() override;
};
