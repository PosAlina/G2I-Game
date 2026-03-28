#pragma once

#include "CoreMinimal.h"
#include "G2IAnswerInterface.h"
#include "G2IInteractiveObjectInterface.h"
#include "G2ILockingInterface.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "G2IPressureGaugeValve.generated.h"

UCLASS()
class G2I_API AG2IPressureGaugeValve : public AActor, public IG2IAnswerInterface,
	public IG2IInteractiveObjectInterface, public IG2ILockingInterface
{
	GENERATED_BODY()
	
public:	
	AG2IPressureGaugeValve();

	virtual void Answer_Implementation(bool bIsRight) override;

	virtual void Interact_Implementation(const ACharacter* Interactor) override;
	virtual bool CanInteract_Implementation(const ACharacter* Interactor) override;
	virtual UG2IWorldHintKeyWidgetComponent *GetInteractionKeyHintComponent_Implementation() override;

	virtual void SetIsLocked_Implementation(bool bIsNewLocked) override;
	virtual bool IsLocked_Implementation() override;

protected:
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable, CallInEditor)
	void ToggleValve();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ValveMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UG2IWorldHintKeyWidgetComponent> HintKeyWidgetComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UTimelineComponent> ValveTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Valve|Animation")
	TObjectPtr<UCurveFloat> ValveCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Angle", meta = (ClampMin = "0.0", ClampMax = "360.0", UIMin = "0.0", UIMax = "360.0"))
	TArray<float> Y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Valve")
	bool bIsValveOpen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Valve")
	bool bIsRotating;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Valve")
	bool bCanOpen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<TObjectPtr<AActor>> Puzzle;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLocked = false;

private:
	bool bIsAState;

	UPROPERTY()
	FRotator InitialRotation;

	UFUNCTION()
	void OnTimelineUpdate(const float Value);

	UFUNCTION()
	void OnTimelineFinished();

	void OpenValve();
	void CloseValve();
};
