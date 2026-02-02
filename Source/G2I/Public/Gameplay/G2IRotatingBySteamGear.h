#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/G2ITraceableObectInterface.h"
#include "Components/TimelineComponent.h"
#include "G2IRotatingBySteamGear.generated.h"

UCLASS()
class G2I_API AG2IRotatingBySteamGear : public AActor, public IG2ITraceableObectInterface
{
	GENERATED_BODY()
private:
	UPROPERTY()
	FOnTimelineFloat TimelineUpdate;

	UPROPERTY()
	TObjectPtr<UTimelineComponent> Timeline;

	float TimelineValue;

	float RotationSign;

protected:
	UPROPERTY(EditAnywhere, Category = "Data")
	TArray<TObjectPtr<AActor>> MovableObjects;

	UPROPERTY(EditAnywhere, Category = "Data|SteamTimelineData")
	TObjectPtr<UCurveFloat> TimelineCurve;

	UPROPERTY(EditAnywhere, Category = "Data")
	float RotationSpeed;

	UFUNCTION(BlueprintCallable, Category = "Timeline")
	void OnTimelineUpdate(float Output);

	UPROPERTY(EditAnywhere, Category = "Data")
	bool bRotateRoll = false;

	UPROPERTY(EditAnywhere, Category = "Data")
	bool bRotatePitch = false;

	UPROPERTY(EditAnywhere, Category = "Data")
	bool bRotateYaw = false;
	
public:
	void OnShoot_Implementation(const FHitResult& HitResult, AActor* Character);
	AG2IRotatingBySteamGear();

	void BeginPlay() override;
};
