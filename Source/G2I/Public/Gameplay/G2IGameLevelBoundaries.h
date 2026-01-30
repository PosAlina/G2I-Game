#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "G2IGameLevelBoundaries.generated.h"

class ATargetPoint;

UCLASS()
class G2I_API AG2IGameLevelBoundaries : public ATriggerBox
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	bool bUseTargetPointRotation = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	TMap<TSubclassOf<AActor>, TObjectPtr<ATargetPoint>> ExitTriggerRespawnLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exit")
	TArray<TObjectPtr<ATriggerBox>> ExitTriggerBoxes;

public:

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	
};
