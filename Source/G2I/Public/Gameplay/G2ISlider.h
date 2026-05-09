#pragma once

#include "CoreMinimal.h"
#include "G2IInteractiveObjectInterface.h"
#include "GameFramework/Actor.h"
#include "G2ISlider.generated.h"

class UG2ISoundComponent;
class AG2IPlayerController;
class UG2ILauncherComponent;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class UG2IColorZoneComponent;
class UG2ISliderLampComponent;
enum class EZoneColor : uint8;
class UBoxComponent;
class FTimerManager;



UENUM(BlueprintType)
enum class EG2ISliderDirection : uint8
{
	Up UMETA(DisplayName = "Up"),
	Down UMETA(DisplayName = "Down")
};

UCLASS()
class G2I_API AG2ISlider : public AActor, public IG2IInteractiveObjectInterface
{
	GENERATED_BODY()
	
public:	
	AG2ISlider();
	virtual void Interact_Implementation(const ACharacter* Interactor) override;
	virtual bool CanInteract_Implementation(const ACharacter* Interactor) override;
	virtual UG2IWorldHintKeyWidgetComponent *GetInteractionKeyHintComponent_Implementation() override;

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

	void Exit();
	void SliderExit();
	
	void CheckErrors();
	void FindAndSwitchLamp();
	void FindLamps();
	void SetImpulse();

	void SetupDefaults();
	void BindDelegates();

	void SliderPush();
	void SelectColor();

	void SoundComponentPlay(const int32 SoundID) const;

	void PlayMovingSliderSound();
	void StopMovingSliderSound();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	FName SliderMoveSoundName = FName("SliderMoveSound");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> SliderBaseSM;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> SliderSM;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> SliderCol;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UG2ILauncherComponent> LauncherComp;

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LampActivationTime = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LampFlashCount = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LampFlashFrequency = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LampErrorTime = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LampErrorFlashCount = 4;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ImpulsePower = 17.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ImpulseDeclinePower = 0.23f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ImpulseDeclineFrequency = 0.008f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SliderPushLength = 5.0f;
	float SliderZStartLocation;
	bool bIsSliderPush = false;
	EG2ISliderDirection SliderPushDirection = EG2ISliderDirection::Down;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SliderPushSpeed = 0.15f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UG2ISoundComponent> SoundComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CorrectSoundName = FName("CorrectSound");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ErrorSoundName = FName("ErrorSound");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> SliderHelpList;
	
protected:
	UPROPERTY(EditAnywhere)
	TSet<TObjectPtr<ACharacter>> PossibleInteractors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UG2IWorldHintKeyWidgetComponent> HintKeyWidgetComp;
	
private:
	bool bIsSliderActive = false;
	bool bIsPuzzleComplete = false;
	int32 IndexInCorrectSequence = 0;
	float CurrenImpulse = 0.0f;
	float MoveDir = 0.0f;
	float CurrentImpulseLenght = 0.0f;
	bool bIsLampWithoutZone = false;
	bool bIsSequenceEmpty = false;
	int32 StopTimerLampsIndex = -1;

	UPROPERTY()
	TObjectPtr<AActor> OriginalViewTarget;
	UPROPERTY()
	TObjectPtr<AG2IPlayerController> PlayerController;
	UPROPERTY()
	TObjectPtr<UWorld> World;
	FTimerHandle ActivationZoneTimer;
	FTimerHandle ImpulseTimer;
	UPROPERTY()
	TMap<EZoneColor, TObjectPtr<UG2ISliderLampComponent>> Lamps;
	UPROPERTY()
	TObjectPtr<UG2ISliderLampComponent> CurrentLamp;
	UPROPERTY()
	TObjectPtr<UG2IColorZoneComponent> CurrentCommonColorZone;
	UPROPERTY()
	TObjectPtr<UG2IColorZoneComponent> CurrentActivationColorZone;
	FTimerHandle SliderPushTimer;
	int32 CorrectSoundID = -1;
	int32 ErrorSoundID = -1;
	int32 SliderMovingSoundId = -1;
};
