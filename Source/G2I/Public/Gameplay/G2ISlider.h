

#pragma once

#include "CoreMinimal.h"
#include "G2IInteractiveObjectInterface.h"
#include "GameFramework/Actor.h"
#include "G2ISlider.generated.h"

struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class UG2IColorZoneComponent;
class UG2ISliderLampComponent;
enum class EZoneColor : uint8;
class UBoxComponent;
class FTimerManager;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPuzzleCompleteDelegate);
UCLASS()
class G2I_API AG2ISlider : public AActor, public IG2IInteractiveObjectInterface
{
	GENERATED_BODY()
	
public:	
	AG2ISlider();
	virtual void Interact_Implementation(const ACharacter* Interactor) override;
	virtual bool CanInteract_Implementation(const ACharacter* Interactor) override;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnSliderBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult);

	UFUNCTION()
	void OnSliderEndOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex);

	void CompareZoneColorToColorInSequence();
	void MoveSlider(const FInputActionValue& Value);
	void MoveSliderImpulse(const FInputActionValue& Value);
	void SliderExit(const FInputActionValue& Value);
	void CheckErrors();
	void FindAndSwitchLamp();
	void FindLamps();
	void SetImpulse();
	
public:
	UPROPERTY(BlueprintAssignable)
	FPuzzleCompleteDelegate OnPuzzleComplete;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> SliderBaseSM;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> SliderSM;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> SliderCol;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MakeEditWidget = true))
	FVector SliderStartLocation = {0.0f, 0.0f, 0.0f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MakeEditWidget = true))
	FVector SliderEndLocation = {0.0f, 0.0f, 0.0f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SliderMoveSpeed = 35.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BlendTime = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EZoneColor> CorrectSequence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> ViewCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UInputMappingContext> SliderIMC;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UInputMappingContext> DefaultIMC;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UInputAction> MoveSliderAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UInputAction> SliderExitAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LampActivationTime = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int LampFlashCount = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LampFlashFrequency = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LampErrorTime = 1.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ImpulsePower = 17.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ImpulseDeclinePower = 0.23f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ImpulseDeclineFrequency = 0.008f;
	
protected:
	UPROPERTY(EditAnywhere)
	TSet<TObjectPtr<ACharacter>> PossibleInteractors;
	
private:
	bool bIsSliderActive = false;
	bool bIsPuzzleComplete = false;
	int IndexInCorrectSequence = 0;
	float CurrenImpulse = 0.0f;
	float MoveDir = 0.0f;
	float CurrentImpulseLenght = 0.0f;
	bool bIsLampWithoutZone = false;
	bool bIsSequenceEmpty = false;
	UPROPERTY()
	TObjectPtr<AActor> OriginalViewTarget;
	UPROPERTY()
	TObjectPtr<APlayerController> PC;
	UPROPERTY()
	TObjectPtr<UWorld> World;
	UPROPERTY()
	FTimerHandle ActivationZoneTimer;
	UPROPERTY()
	FTimerHandle ImpulseTimer;
	UPROPERTY()
	TMap<EZoneColor, TObjectPtr<UG2ISliderLampComponent>> Lamps;
	UPROPERTY()
	TObjectPtr<UG2ISliderLampComponent> CurrentLamp;
	UPROPERTY()
	TObjectPtr<UG2IColorZoneComponent> CurrentCommonColorZone;
	UPROPERTY()
	TObjectPtr<UG2IColorZoneComponent> CurrentActivationColorZone;
	FTimerManager* TimerManager;
};
