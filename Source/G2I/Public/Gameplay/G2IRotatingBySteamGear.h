#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/G2ITraceableObectInterface.h"
#include "Components/TimelineComponent.h"
#include "G2IRotatingBySteamGear.generated.h"


class AG2IRotatingBySteamGear;
class UG2ISoundComponent;
class UG2IOutlineComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FStartRotateDelegate, AG2IRotatingBySteamGear*);

UCLASS()
class G2I_API AG2IRotatingBySteamGear : public AActor, public IG2ITraceableObectInterface
{
	
	GENERATED_BODY()
	
private:
	FOnTimelineFloat TimelineUpdate;
	FOnTimelineEvent TimelineFinished;

	UPROPERTY()
	TObjectPtr<UTimelineComponent> Timeline;

	float TimelineValue;

	float RotationSign;

	int32 GearRotationSoundId = -1;

protected:
	UPROPERTY(EditAnywhere, Category = "Data")
	TArray<TObjectPtr<AActor>> MovableObjects;

	UPROPERTY(EditAnywhere, Category = "Data|SteamTimelineData")
	TObjectPtr<UCurveFloat> TimelineCurve;

	UPROPERTY(EditAnywhere, Category = "Data")
	float RotationSpeed;

	UFUNCTION(BlueprintCallable, Category = "Timeline")
	void OnTimelineUpdate(float Output);

	UFUNCTION(BlueprintCallable, Category = "Timeline")
	void OnTimelineFinished();

	UPROPERTY(EditAnywhere, Category = "Data")
	bool bRotateRoll = false;

	UPROPERTY(EditAnywhere, Category = "Data")
	bool bRotatePitch = false;

	UPROPERTY(EditAnywhere, Category = "Data")
	bool bRotateYaw = false;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	FName RequiredItemID;

	UPROPERTY(EditAnywhere, Category = "Outliner")
	TObjectPtr<UG2IOutlineComponent> OutlineComponent;

	virtual void BeginPlay() override;
	
public:
	
	FStartRotateDelegate OnStartRotateDelegate;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<UG2ISoundComponent> SoundComp;

	virtual void OnShoot_Implementation(const FHitResult& HitResult, AActor* Character) override;
	AG2IRotatingBySteamGear();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Repair(AActor* Interactor);

	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool IsActive = false;
};
