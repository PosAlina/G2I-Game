

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

UCLASS()
class G2I_API AG2ISlider : public AActor, public IG2IInteractiveObjectInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AG2ISlider();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> SliderBaseSM;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> SliderSM;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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

	int IndexInCorrectSequence = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCameraComponent> ViewCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputMappingContext> SliderIMC;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputMappingContext> DefaultIMC;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputAction> MoveSliderAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputAction> SliderExitAction;

private:
	bool bIsSliderActive = false;
	bool bIsPuzzleComplete = false;
	UPROPERTY()
	TObjectPtr<AActor> OriginalViewTarget;
	UPROPERTY()
	TObjectPtr<APlayerController> PC;
	UPROPERTY()
	TObjectPtr<UWorld> World;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Interact_Implementation(const ACharacter* Interactor) override;
	virtual bool CanInteract_Implementation(const ACharacter* Interactor) override;

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

	bool CompareZoneColorToColorInSequence(UG2IColorZoneComponent& CurrentColorZone);
	void MoveSlider(const FInputActionValue& Value);
	void MoveSliderInertia(const FInputActionValue& Value);
	void SliderExit(const FInputActionValue& Value);
	void FindAndSwitchLamp(bool bLampMode, UG2IColorZoneComponent& CurrentColorZone);
};
