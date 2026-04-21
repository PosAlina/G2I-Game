#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/G2IInteractiveObjectInterface.h"
#include "Interfaces/G2ITraceableObectInterface.h"
#include "Interfaces/G2IMovingObjectInterface.h"
#include "Components/TimelineComponent.h"
#include "G2IMovingBySteamAndHandsObject.generated.h"

class UG2IOutlineComponent;

UCLASS()
class G2I_API AG2IMovingBySteamAndHandsObject : public AActor, public IG2ITraceableObectInterface, public IG2IInteractiveObjectInterface, public IG2IMovingObjectInterface
{
	GENERATED_BODY()

private:
	FOnTimelineFloat TimelineUpdate;

	FVector ForwardVector;

	float Multiplier;
	
	UPROPERTY()
	TObjectPtr<UTimelineComponent> Timeline;

	float TimelineValue;
protected:	
	AG2IMovingBySteamAndHandsObject();

	UPROPERTY(EditAnywhere, Category = "Data|SteamTimelineData")
	TObjectPtr<UCurveFloat> TimelineCurve;

	UPROPERTY(EditAnywhere, Category = "Data")
	TSet<TObjectPtr<ACharacter>> PossibleInteractors;

	UPROPERTY(EditAnywhere, Category = "Data")
	float BounceMultiplier;

	UPROPERTY(EditAnywhere, Category = "Data")
	float SteamPushForce;
	
	UPROPERTY(EditAnywhere, Category = "Outliner")
	TObjectPtr<UG2IOutlineComponent> OutlineComponent;

	UFUNCTION(BlueprintCallable, Category = "Timeline")
	void OnTimelineUpdate(float Output);

	virtual void BeginPlay() override;
public:
	virtual bool CanInteract_Implementation(const ACharacter* Interactor) override;
	virtual void Interact_Implementation(const ACharacter* Interactor) override;
	virtual void OnShoot_Implementation(const FHitResult& HitResult, AActor* Character) override;
	virtual float GetSpeedChange_Implementation() override;
};
