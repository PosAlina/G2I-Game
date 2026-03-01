#include "Gameplay/G2ISlider.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "G2I.h"
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
	
	if (!ensure(SliderBaseSM))
	{
		UE_LOG(LogG2I, Error, TEXT("SliderBaseSM was not created in %s"), *GetActorNameOrLabel());
		return;
	}
	
	if (!ensure(SliderSM))
	{
		UE_LOG(LogG2I, Error, TEXT("SliderSM was not created in %s"), *GetActorNameOrLabel());
		return;
	}
	
	if (!ensure(SliderCol))
	{
		UE_LOG(LogG2I, Error, TEXT("LampMesh was not created in %s"), *GetActorNameOrLabel());
		return;
	}
	
	if (!ensure(ViewCamera))
	{
		UE_LOG(LogG2I, Error, TEXT("ViewCamera was not created in %s"), *GetActorNameOrLabel());
		return;
	}
	
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
	
	World =  GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World is null in %s"), *GetActorNameOrLabel());
		return;
	}

	PC = World->GetFirstPlayerController();

	if (!ensure(PC))
	{
		UE_LOG(LogG2I, Error, TEXT("PC is null in %s"), *GetActorNameOrLabel());
		return;
	}

	if (SliderSM)
	{
		SliderSM->SetRelativeLocation(SliderStartLocation);
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
	
	if (ensure(PC) && !bIsSliderActive)
	{
		auto* Subsystem = PC->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (!Subsystem)
		{
			UE_LOG(LogG2I, Error, TEXT("Sybsystem is null in %s"), *GetActorNameOrLabel());
			return;
		}
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
		UE_LOG(LogG2I, Warning, TEXT("There is lamp without color zone in %s"), *GetActorNameOrLabel());
		return false;
	}
	if (bIsSequenceEmpty)
	{
		UE_LOG(LogG2I, Warning, TEXT("Correct Sequense is empty in %s"), *GetActorNameOrLabel());
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
		if (Comp && !Lamps.Contains(Comp->Color))
		{
			bIsLampWithoutZone = true;
			break;
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
	if (!OtherComp)
	{
		UE_LOG(LogG2I, Log, TEXT("%s has not components"), *OtherActor->GetName());
		return;
	}
	
	auto* tempColorZone = Cast<UG2IColorZoneComponent>(OtherComp->GetAttachParent());

	if (!tempColorZone)
	{
		UE_LOG(LogG2I, Log, TEXT("OtherComp is not ColorZoneComponent in %s"), *GetActorNameOrLabel());
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

	if (CurrentLamp)
	{
		if (CurrentActivationColorZone)
		{
			CurrentLamp->LampMode = 2;
			GetWorldTimerManager().SetTimer(ActivationZoneTimer, this, &ThisClass::CompareZoneColorToColorInSequence, LampActivationTime, false);
			CurrentLamp->SetTimerToIntensity(1);
		}
		else
		{
			CurrentLamp->LampMode = 1;
			CurrentLamp->SetTimerToIntensity(1);
		}
	}
}

void AG2ISlider::OnSliderEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherComp)
	{
		UE_LOG(LogG2I, Log, TEXT("%s has not components"), *OtherActor->GetName());
		return;
	}
	
	auto* tempColorZone = Cast<UG2IColorZoneComponent>(OtherComp->GetAttachParent());

	if (!tempColorZone)
	{
		UE_LOG(LogG2I, Log, TEXT("OtherComp is not ColorZoneComponent in %s"), *GetActorNameOrLabel());
		return;
	}

	if (CurrentLamp)
	{
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
}

void AG2ISlider::CompareZoneColorToColorInSequence()
{
	if (!bIsPuzzleComplete && CurrentActivationColorZone && CurrentLamp)
	{
		if (CorrectSequence[IndexInCorrectSequence] == CurrentActivationColorZone->Color)
		{
			#if WITH_EDITOR
				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange,FString::Printf(TEXT("Correct lamp, %i out of %i"), IndexInCorrectSequence+1, CorrectSequence.Num()));
			#endif
			UE_LOG(LogG2I, Log, TEXT("Correct lamp, %i out of %i"), IndexInCorrectSequence+1, CorrectSequence.Num());
			
			IndexInCorrectSequence++;
			if (IndexInCorrectSequence == CorrectSequence.Num())
			{
				#if WITH_EDITOR
					GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Sequence is correct");
				#endif
				UE_LOG(LogG2I, Log, TEXT("Sequence is correct"));
				bIsPuzzleComplete = true;
				OnPuzzleComplete.Broadcast();
			}
			CurrentLamp->SetTimerToFlashing(LampFlashCount, LampFlashFrequency);
		}
		else
		{
			#if WITH_EDITOR
				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, "Wrong lamp, start over");
			#endif
			UE_LOG(LogG2I, Log, TEXT("Wrong lamp, start over"));
			CurrentLamp->SetTimerToFlashing(1, LampErrorTime);
			IndexInCorrectSequence = 0;
		}
	}
	GetWorldTimerManager().ClearTimer(ActivationZoneTimer);
}

void AG2ISlider::MoveSlider(const FInputActionValue& Value)
{
	if (bIsSliderActive && SliderSM)
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
	if (ensure(PC) && bIsSliderActive)
	{
		auto* Subsystem = PC->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (!Subsystem)
		{
			UE_LOG(LogG2I, Error, TEXT("Sybsystem is null in %s"), *GetActorNameOrLabel());
			return;
		}
		PC->SetViewTargetWithBlend(OriginalViewTarget, BlendTime);
		bIsSliderActive = false;
		Subsystem->RemoveMappingContext(SliderIMC);
		Subsystem->AddMappingContext(DefaultIMC, 0);
	}
}

void AG2ISlider::FindAndSwitchLamp()
{
	if (CurrentCommonColorZone && !CurrentCommonColorZone->bIsActivationZone)
	{
		CurrentLamp = Lamps.FindRef(CurrentCommonColorZone->Color);
		if (CurrentLamp && CurrentLamp->DynamicMaterial)
		{
			CurrentLamp->DynamicMaterial->SetVectorParameterValue("EmissiveColor", CurrentLamp->LampColor);
		}
	}
}

void AG2ISlider::FindLamps()
{
	TArray<UG2ISliderLampComponent*> LampComponents;
	GetComponents(LampComponents);

	for (auto* Comp : LampComponents)
	{
		if (Comp)
		{
			Lamps.Add(Comp->Color, Comp);
		}
	}
}

void AG2ISlider::SetImpulse()
{
	if (SliderSM)
	{
		float SliderOffset = SliderSM->GetRelativeLocation().Y + MoveDir*CurrenImpulse*World->DeltaTimeSeconds;
		if (SliderOffset < SliderStartLocation.Y || SliderOffset > SliderEndLocation.Y)
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
}
