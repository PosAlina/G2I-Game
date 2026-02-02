

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/G2IInteractiveObjectInterface.h"
#include "Interfaces/G2ITraceableObectInterface.h"
#include "Interfaces/G2IMovingObjectInterface.h"
#include "Components/TimelineComponent.h"
#include "G2IMovingBySteamAndHandsObject.generated.h"

class UPhysicsConstraintComponent;
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

	UFUNCTION(BlueprintCallable, Category = "Timeline")
	void OnTimelineUpdate(float Output);

	void BeginPlay() override;
public:
	bool CanInteract_Implementation(const ACharacter* Interactor) override;
	void Interact_Implementation(const ACharacter* Interactor) override;
	void OnShoot_Implementation(const FHitResult& HitResult, AActor* Character) override;
	float GetSpeedChange_Implementation() override;
};
