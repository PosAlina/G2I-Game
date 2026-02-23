

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "G2IPressureGaugeValve.generated.h"

UCLASS()
class G2I_API AG2IPressureGaugeValve : public AActor
{
	GENERATED_BODY()
	
public:	
	AG2IPressureGaugeValve();

protected:
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable, CallInEditor)
	void ToggleValve();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ValveMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UTimelineComponent> ValveTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Valve|Animation")
	TObjectPtr<UCurveFloat> ValveCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Angle", meta = (ClampMin = "0.0", ClampMax = "360.0", UIMin = "0.0", UIMax = "360.0"))
	TArray<float> Y;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Valve")
	bool bIsValveOpen;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Valve")
	bool bCanOpen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<TObjectPtr<AActor>> Puzzle;

private:
	bool bIsAState;
	bool bIsRotating;

	UFUNCTION()
	void OnTimelineUpdate(float Value);

	UFUNCTION()
	void OnTimelineFinished();

	void OpenValve();
	void CloseValve();
};
