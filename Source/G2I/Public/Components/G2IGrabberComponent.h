

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/G2IActivationInterface.h"
#include "G2IGrabberComponent.generated.h"

class UG2IVerticalMovementComponent;
class UPhysicsHandleComponent;
class UBoxComponent;
class USceneComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2IGrabberComponent : public USceneComponent, public IG2IActivationInterface
{
	GENERATED_BODY()
private:
	FTimerHandle MovementTimer;
	FVector StartLocation = FVector::ZeroVector;
	FVector TargetLocation = FVector::ZeroVector;
	float CurrentGrabTime = 0.0f;

	UPROPERTY()
	TObjectPtr<UWorld> World;
	UPROPERTY()
	TObjectPtr<AActor> Owner;
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComp;
	UPROPERTY()
	TObjectPtr<AActor> GrabbedActor;

	bool bHookIsClosed = false;

	void UpdateSmoothMovement();

	void PlayHookAnimation(UAnimSequence* Animation);

	void FinishGrab(UPrimitiveComponent* OtherComp, FVector Location, AActor* OtherActor);

	UFUNCTION()
	void OnMoveDownFinished();
public:	
	UG2IGrabberComponent();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|Grabber|Movement")
	TObjectPtr<UG2IVerticalMovementComponent> VerticalMovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|Grabber")
	TObjectPtr<UPhysicsHandleComponent> PhysicsHandleComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimSequence> OpenHookAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimSequence> CloseHookAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Grabber", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> Anchor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Grabber", meta = (ToolTip = "Don't change without a reason"))
	float UpdateTime = 0.016f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Grabber", meta = (ToolTip = "Variable for initial movement while closing grabber"))
	float MaxGrabTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Grabber", meta = (ToolTip = "If true, physics handle will be removed immediately after grabbing the object"))
	bool bRemovePhysicsHandleAfterGrab = false;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;

	void Activate_Implementation() override;
	void Deactivate_Implementation() override;

};
