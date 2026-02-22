#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "G2IActorsActivatorByOverlappingActors.generated.h"

USTRUCT(BlueprintType)
struct FG2IActivatedActors
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Activation)
	TSet<TObjectPtr<AActor>> SetOfActivatedActors;;
};

/**
 * When actors with the required tag enter this zone, actors from the required set are activated
 */
UCLASS()
class G2I_API AG2IActorsActivatorByOverlappingActors : public ATriggerBox
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlapping activator")
	TMap<FName, FG2IActivatedActors> ActivatedActorsByCheckerTag;;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlapping activator")
	bool bEnableActivateBeginOverlappingActors = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlapping activator")
	bool bEnableDeactivateEndOverlappingActors = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlapping activator")
	TObjectPtr<USoundBase> ActivateSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlapping activator")
	TObjectPtr<USoundBase> DeactivateSound;

private:

	UPROPERTY()
	TObjectPtr<UWorld> World;
	
public:

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	bool ActivateActorsByActor(const AActor* Activator);

	bool DeactivateActorsByActor(const AActor *Activator);
	
protected:

	virtual void BeginPlay() override;

};