#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Gameplay/G2IRespawnPoint.h"
#include "G2IGameLevelBoundaries.generated.h"

UCLASS()
class G2I_API AG2IGameLevelBoundaries : public ATriggerBox
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	bool bUseTargetPointRotation = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	TMap<TSubclassOf<AActor>, TObjectPtr<AG2IRespawnPoint>> ExitTriggerRespawnLocations;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exit")
	TArray<TObjectPtr<ATriggerBox>> ExitTriggerBoxes;

public:

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	
};
