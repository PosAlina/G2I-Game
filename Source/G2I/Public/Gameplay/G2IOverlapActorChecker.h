#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "G2IOverlapActorChecker.generated.h"

UCLASS()
class G2I_API AG2IOverlapActorChecker : public ATriggerBox
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overlapping)
	FName CheckerTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overlapping)
	bool bEnableActivateBeginOverlappingActors = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overlapping)
	bool bEnableDeactivateEndOverlappingActors = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overlapping)
	TObjectPtr<USoundBase> ActivateSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overlapping)
	TObjectPtr<USoundBase> DeactivateSound;

private:

	UPROPERTY()
	TObjectPtr<UWorld> World;
	
public:

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

protected:

	virtual void BeginPlay() override;

};
