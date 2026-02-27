


#include "Gameplay/G2ISlider.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "G2IColorZoneComponent.h"
#include "G2ISliderLampComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"

AG2ISlider::AG2ISlider()
{
	SliderBaseSM = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SliderBase"));
	SliderSM = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Slider"));
	SliderCol = CreateDefaultSubobject<UBoxComponent>(TEXT("SliderCol"));
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));

	SliderBaseSM->SetupAttachment(RootComponent);
	SliderSM->SetupAttachment(SliderBaseSM);
	SliderCol->SetupAttachment(SliderSM);
	ViewCamera->SetupAttachment(SliderBaseSM);

	SliderCol->OnComponentBeginOverlap.AddDynamic(this, &AG2ISlider::OnSliderBeginOverlap);
	SliderCol->OnComponentEndOverlap.AddDynamic(this, &AG2ISlider::OnSliderEndOverlap);
}

void AG2ISlider::BeginPlay()
{
	Super::BeginPlay();
	SliderSM->SetRelativeLocation(SliderStartLocation);
	
	World =  GetWorld();
	if (World == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("World is nullptr"));
		return;
	}

	PC = World->GetFirstPlayerController();

	if (PC == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PC is nullptr"));
		return;
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PC->InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveSliderAction, ETriggerEvent::Triggered, this, &ThisClass::MoveSlider);
		EnhancedInputComponent->BindAction(MoveSliderAction, ETriggerEvent::Completed, this, &ThisClass::MoveSliderImpulse);
		EnhancedInputComponent->BindAction(SliderExitAction, ETriggerEvent::Started, this, &ThisClass::SliderExit);
	}

	FindLamps();
	CheckErrors();
	CurrenImpulse = ImpulsePower;
}

void AG2ISlider::Interact_Implementation(const ACharacter* Interactor)
{
	IG2IInteractiveObjectInterface::Interact_Implementation(Interactor);
	
	if (!bIsSliderActive)
	{
		auto Subsystem  = PC->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		OriginalViewTarget = PC->GetViewTarget();
		PC->SetViewTargetWithBlend(this, BlendTime);
		bIsSliderActive = true;
		Subsystem->AddMappingContext(SliderIMC, 0);
		Subsystem->RemoveMappingContext(DefaultIMC);
	}
}

bool AG2ISlider::CanInteract_Implementation(const ACharacter* Interactor)
{
	if (bIsLampWithoutZone)
	{
		UE_LOG(LogTemp, Warning, TEXT("There is lamp without color zone"));
		return false;
	}
	if (bIsSequenceEmpty)
	{
		UE_LOG(LogTemp, Warning, TEXT("Correct Sequense is empty"));
		return false;
	}
	return true;
}

void AG2ISlider::CheckErrors()
{
	TArray<TObjectPtr<UG2IColorZoneComponent>> ColorZones;
	GetComponents(ColorZones);

	for (auto Comp : ColorZones)
	{
		if (!Lamps.Contains(Comp->Color))
		{
			bIsLampWithoutZone = true;
		}
	}

	if (CorrectSequence.Num() == 0)
	{
		bIsSequenceEmpty = true;
	}
}

void AG2ISlider::OnSliderBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto tempColorZone = Cast<UG2IColorZoneComponent>(OtherComp->GetAttachParent());

	if (!tempColorZone)
	{
		UE_LOG(LogTemp, Log, TEXT("OtherComp is not ColorZoneComponent"));
		return;
	}
	
	if (tempColorZone->bIsActivationZone)
	{
		CurrentActivationColorZone = tempColorZone;
	}
	else
	{
		CurrentCommonColorZone = tempColorZone;
	}

	FindAndSwitchLamp();
	
	if (CurrentActivationColorZone)
	{
		CurrentLamp->LampMode = 2;
		GetWorldTimerManager().SetTimer(ActivationZoneTimer, this, &ThisClass::CompareZoneColorToColorInSequence, 1.5f, false);
		CurrentLamp->SetTimerToIntensity(1);
	}
	else
	{
		CurrentLamp->LampMode = 1;
		CurrentLamp->SetTimerToIntensity(1);
	}
}

void AG2ISlider::OnSliderEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto tempColorZone = Cast<UG2IColorZoneComponent>(OtherComp->GetAttachParent());

	if (!tempColorZone)
	{
		UE_LOG(LogTemp, Log, TEXT("CurrentColorZoneCol hasnt ColorZone"));
		return;
	}

	if (CurrentActivationColorZone)
	{
		CurrentActivationColorZone = nullptr;
		CurrentLamp->LampMode = 1;
	}
	else
	{
		CurrentCommonColorZone = nullptr;
		CurrentLamp->LampMode = 0;
	}

	if (!CurrentLamp->bIsLampFlashing)
	{
		CurrentLamp->SetTimerToIntensity(-1);
	}
}

void AG2ISlider::CompareZoneColorToColorInSequence()
{
	if (!bIsPuzzleComplete && CurrentActivationColorZone && CorrectSequence[IndexInCorrectSequence] == CurrentActivationColorZone->Color)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, "Correct");
		IndexInCorrectSequence++;
		if (IndexInCorrectSequence == CorrectSequence.Num())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Correct All");
			bIsPuzzleComplete = true;
		}
		CurrentLamp->SetTimerToFlashing(LampFlashCount, LampFlashFrequency);
	}
	else if (!bIsPuzzleComplete && CurrentActivationColorZone && CorrectSequence[IndexInCorrectSequence] != CurrentActivationColorZone->Color)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, "NoCorrect");
		CurrentLamp->SetTimerToFlashing(1, LampErrorTime);
		IndexInCorrectSequence = 0;
	}
	GetWorldTimerManager().ClearTimer(ActivationZoneTimer);
}

void AG2ISlider::MoveSlider(const FInputActionValue& Value)
{
	if (bIsSliderActive)
	{
		GetWorldTimerManager().ClearTimer(ImpulseTimer);
		CurrenImpulse = ImpulsePower;
		MoveDir = Value.Get<float>();
		FVector SliderLocation = SliderSM->GetRelativeLocation();
		FVector OffsetVector = {0.0f, SliderMoveSpeed*World->DeltaTimeSeconds, 0.0f};

		if (MoveDir > 0.0f)
		{
			if (SliderLocation.Y < SliderEndLocation.Y)
			{
				SliderSM->AddLocalOffset(OffsetVector);
			}
		}
		else
		{
			if (SliderLocation.Y > SliderStartLocation.Y)
			{
				SliderSM->AddLocalOffset(-OffsetVector);
			}
		}
	}
}

void AG2ISlider::MoveSliderImpulse(const FInputActionValue& Value)
{
	if (bIsSliderActive)
	{
		GetWorldTimerManager().SetTimer(ImpulseTimer, [this]()
		{
			SetImpulse();
		}, ImpulseDeclineFrequency, true);
	}
}

void AG2ISlider::SliderExit(const FInputActionValue& Value)
{
	if (bIsSliderActive)
	{
		auto Subsystem  = PC->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		PC->SetViewTargetWithBlend(OriginalViewTarget, BlendTime);
		bIsSliderActive = false;
		Subsystem->RemoveMappingContext(SliderIMC);
		Subsystem->AddMappingContext(DefaultIMC, 0);
	}
}

void AG2ISlider::FindAndSwitchLamp()
{
	if (!CurrentCommonColorZone->bIsActivationZone)
	{
		CurrentLamp = Lamps.FindRef(CurrentCommonColorZone->Color);
		CurrentLamp->DynamicMaterial->SetVectorParameterValue("EmissiveColor", CurrentLamp->LampColor);
	}
}

void AG2ISlider::FindLamps()
{
	TArray<UG2ISliderLampComponent*> LampComponents;
	GetComponents(LampComponents);

	for (auto Comp : LampComponents)
	{
		Lamps.Add(Comp->Color, Comp);
	}
}

void AG2ISlider::SetImpulse()
{
	float temp = SliderSM->GetRelativeLocation().Y + MoveDir*CurrenImpulse*World->DeltaTimeSeconds;
	if (temp < SliderStartLocation.Y || temp > SliderEndLocation.Y)
	{
		GetWorldTimerManager().ClearTimer(ImpulseTimer);
		return;
	}
	
	SliderSM->AddLocalOffset({0.0f, MoveDir*CurrenImpulse*World->DeltaTimeSeconds, 0.0f});
	CurrenImpulse -= ImpulseDeclinePower;
	if (CurrenImpulse < 0.0f)
	{
		GetWorldTimerManager().ClearTimer(ImpulseTimer);
		CurrenImpulse = ImpulsePower;
	}
}
