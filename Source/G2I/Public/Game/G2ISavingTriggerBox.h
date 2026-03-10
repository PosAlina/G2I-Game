#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Interfaces/SavingSystem/G2ISavableInterface.h"
#include "Interfaces/SavingSystem/G2ISaveGameplayInterface.h"
#include "G2ISavingTriggerBox.generated.h"

/**
 * Trigger box that, when triggered by the player, saves gameplay & deletes itself
 */
UCLASS()
class G2I_API AG2ISavingTriggerBox : public ATriggerBox, public IG2ISavableInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UGameInstance> GameInstance;

	UPROPERTY()
	bool bActivated = false;

public:
	AG2ISavingTriggerBox();

	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnGameplaySaved(bool bSuccess);

	void SaveData_Implementation(UG2IGameplaySaveGame* SaveGameRef);

	void LoadData_Implementation(const UG2IGameplaySaveGame* SaveGameRef);

protected:
	virtual void BeginPlay() override;
};
