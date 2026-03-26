#include "Gameplay/G2ISlider.h"
#include "EnhancedInputComponent.h"
#include "G2I.h"
#include "G2ICharacterEngineer.h"
#include "G2IColorZoneComponent.h"
#include "G2IPlayerController.h"
#include "G2ISliderLampComponent.h"
#include "G2IWorldHintKeyWidgetComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "LaunchingIndication/G2ILauncherComponent.h"

AG2ISlider::AG2ISlider()
{
	SliderBaseSM = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SliderBase"));
	SliderSM = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Slider"));
	SliderCol = CreateDefaultSubobject<UBoxComponent>(TEXT("SliderCol"));
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	LauncherComp = CreateDefaultSubobject<UG2ILauncherComponent>(TEXT("LauncherComp"));
	HintKeyWidgetComp = CreateDefaultSubobject<UG2IWorldHintKeyWidgetComponent>(TEXT("HintKeyWidget"));
	if (!ensure(HintKeyWidgetComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create %s"), *GetActorNameOrLabel(),
			*UG2IWorldHintKeyWidgetComponent::StaticClass()->GetName());
	}
	else
	{
		HintKeyWidgetComp->SetupAttachment(RootComponent);
	}
	if (!ensure(LauncherComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*UG2ILauncherComponent::StaticClass()->GetName());
	}
	
	if (!ensure(SliderBaseSM))
	{
		UE_LOG(LogG2I, Error, TEXT("SliderBaseSM was not created in %s"), *GetActorNameOrLabel());
		return;
	}
	SliderBaseSM->SetupAttachment(RootComponent);
	
	if (!ensure(SliderSM))
	{
		UE_LOG(LogG2I, Error, TEXT("SliderSM was not created in %s"), *GetActorNameOrLabel());
		return;
	}
	SliderSM->SetupAttachment(SliderBaseSM);
	
	if (!ensure(SliderCol))
	{
		UE_LOG(LogG2I, Error, TEXT("LampMesh was not created in %s"), *GetActorNameOrLabel());
		return;
	}
	SliderCol->SetupAttachment(SliderSM);
	SliderCol->OnComponentBeginOverlap.AddDynamic(this, &AG2ISlider::OnSliderBeginOverlap);
	SliderCol->OnComponentEndOverlap.AddDynamic(this, &AG2ISlider::OnSliderEndOverlap);
	
	if (!ensure(ViewCamera))
	{
		UE_LOG(LogG2I, Error, TEXT("ViewCamera was not created in %s"), *GetActorNameOrLabel());
		return;
	}
	ViewCamera->SetupAttachment(SliderBaseSM);
}

void AG2ISlider::BeginPlay()
{
	Super::BeginPlay();
	
	SetupDefaults();
	BindDelegates();
}

void AG2ISlider::SetupDefaults()
{
	if (!ensure(HintKeyWidgetComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create %s"), *GetActorNameOrLabel(),
			*UG2IWorldHintKeyWidgetComponent::StaticClass()->GetName());
	}
	else
	{
		if (!ensure(LauncherComp))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
				*UG2ILauncherComponent::StaticClass()->GetName());
		}
		else
		{
			LauncherComp->SetHintKeyWidget(HintKeyWidgetComp);
		}
	}
	
	World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World is null in %s"), *GetActorNameOrLabel());
		return;
	}

	PlayerController = Cast<AG2IPlayerController>(World->GetFirstPlayerController());
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("PlayerController is null in %s"), *GetActorNameOrLabel());
		return;
	}

	if (!ensure(SliderSM))
	{
		UE_LOG(LogG2I, Error, TEXT("Couldn't find SliderSM in %s"), *GetActorNameOrLabel());
		return;
	}
	SliderSM->SetRelativeLocation(SliderStartLocation);

	FindLamps();
	CheckErrors();
	CurrenImpulse = ImpulsePower;
}

void AG2ISlider::BindDelegates()
{
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
	if (!ensure(EnhancedInputComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Enhanced Input Component"), *GetActorNameOrLabel());
		return;
	}
	EnhancedInputComponent->BindAction(MoveSliderAction, ETriggerEvent::Triggered, this, &ThisClass::MoveSlider);
	EnhancedInputComponent->BindAction(MoveSliderAction, ETriggerEvent::Completed, this, &ThisClass::MoveSliderImpulse);
	EnhancedInputComponent->BindAction(SliderExitAction, ETriggerEvent::Started, this, &ThisClass::SliderExit);
}

void AG2ISlider::Interact_Implementation(const ACharacter* Interactor)
{
	if (bIsSliderActive)
	{
		return;
	}
	
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*AG2IPlayerController::StaticClass()->GetName());
		return;
	}
	
	// TODO: Rewrite to use Camera Controller
	OriginalViewTarget = PlayerController->GetViewTarget();
	PlayerController->SetViewTargetWithBlend(this, BlendTime);
	bIsSliderActive = true;
	PlayerController->OverrideInputMappingContext({SliderIMC});

	if (!ensure(HintKeyWidgetComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*UG2IWorldHintKeyWidgetComponent::StaticClass()->GetName());
	}
	else
	{
		HintKeyWidgetComp->SetIsLocked_Implementation(true);
	}
}

bool AG2ISlider::CanInteract_Implementation(const ACharacter* Interactor)
{
	if (!ensure(LauncherComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*UG2ILauncherComponent::StaticClass()->GetName());
	}
	else
	{
		if (LauncherComp->IsLocked_Implementation())
		{
			return false;
		}
	}
	
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
	if (Interactor && PossibleInteractors.Contains(Interactor))
	{
		return true;
	}
	return false;
}

UG2IWorldHintKeyWidgetComponent* AG2ISlider::GetInteractionKeyHintComponent_Implementation()
{
	return HintKeyWidgetComp;
}

void AG2ISlider::CheckErrors()
{
	TArray<TObjectPtr<UG2IColorZoneComponent>> ColorZones;
	GetComponents(ColorZones);

	for (const auto Comp : ColorZones)
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
	
	auto* TempColorZone = Cast<UG2IColorZoneComponent>(OtherComp->GetAttachParent());

	if (!TempColorZone)
	{
		UE_LOG(LogG2I, Log, TEXT("OtherComp is not ColorZoneComponent in %s"), *GetActorNameOrLabel());
		return;
	}
	
	if (TempColorZone->bIsActivationZone)
	{
		CurrentActivationColorZone = TempColorZone;
	}
	else
	{
		CurrentCommonColorZone = TempColorZone;
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

	const auto* TempColorZone = Cast<UG2IColorZoneComponent>(OtherComp->GetAttachParent());

	if (!TempColorZone)
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
	GetWorldTimerManager().ClearTimer(ActivationZoneTimer);
	if (!bIsPuzzleComplete && CurrentActivationColorZone && CurrentLamp)
	{
		if (CorrectSequence[IndexInCorrectSequence] == CurrentActivationColorZone->Color)
		{
#if WITH_EDITOR
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,FString::Printf(TEXT("Correct lamp, %i out of %i"), IndexInCorrectSequence+1, CorrectSequence.Num()));
				}
#endif
			UE_LOG(LogG2I, Log, TEXT("Correct lamp, %i out of %i"), IndexInCorrectSequence+1, CorrectSequence.Num());
			
			IndexInCorrectSequence++;
			if (IndexInCorrectSequence == CorrectSequence.Num())
			{
#if WITH_EDITOR
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Sequence is correct");
				}
#endif
				UE_LOG(LogG2I, Log, TEXT("Sequence is correct"));
				bIsPuzzleComplete = true;
				
				if (!ensure(LauncherComp))
				{
					UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
						*UG2ILauncherComponent::StaticClass()->GetName());
				}
				else
				{
					LauncherComp->SetIsLaunched(true);
				}
				if (!ensure(SliderCol))
				{
					UE_LOG(LogG2I, Error, TEXT("LampMesh was not created in %s"), *GetActorNameOrLabel());
				}
				else
				{
					SliderCol->OnComponentBeginOverlap.RemoveDynamic(this, &AG2ISlider::OnSliderBeginOverlap);
					SliderCol->OnComponentEndOverlap.RemoveDynamic(this, &AG2ISlider::OnSliderEndOverlap);
				}
				for (const auto& [_,Lamp] : Lamps)
				{
					if (!Lamp)
					{
						Exit();
						continue;
					}
					Lamp->OnStopFlashingTimer.Unbind();
					Lamp->OnStopFlashingTimer.BindUObject(this, &ThisClass::Exit);
					Lamp->SetTimerToFlashing(LampFlashFrequency, LampFlashCount);
				}
				Exit();
			}
			else
			{
				CurrentLamp->SetTimerToFlashing(LampFlashFrequency, LampFlashCount);
			}
		}
		else
		{
#if WITH_EDITOR
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Wrong lamp, start over");
			}
#endif
			UE_LOG(LogG2I, Log, TEXT("Wrong lamp, start over"));
			CurrentLamp->SetTimerToFlashing(LampErrorTime, 2);
			IndexInCorrectSequence = 0;
		}
	}
}

void AG2ISlider::MoveSlider(const FInputActionValue& Value)
{
	if (bIsSliderActive && SliderSM)
	{
		GetWorldTimerManager().ClearTimer(ImpulseTimer);
		CurrenImpulse = ImpulsePower;
		MoveDir = Value.Get<float>();
		const FVector SliderLocation = SliderSM->GetRelativeLocation();
		const FVector OffsetVector = {0.0f, SliderMoveSpeed*World->DeltaTimeSeconds, 0.0f};

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
		}, ImpulseDeclineFrequency, true, 0.0f);
	}
}

void AG2ISlider::Exit()
{
	++StopTimerLampsIndex;
	if (StopTimerLampsIndex == Lamps.Num())
	{
		for (const auto& [_,Lamp] : Lamps)
		{
			if (Lamp)
			{
				Lamp->OnStopFlashingTimer.Unbind();
				Lamp->OnLamp();
			}
		}
		SliderExit();
	}
}

void AG2ISlider::SliderExit()
{
	if (!bIsSliderActive)
	{
		return;
	}

	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*AG2IPlayerController::StaticClass()->GetName());
		return;
	}
	
	PlayerController->SetViewTargetWithBlend(OriginalViewTarget, BlendTime);
	bIsSliderActive = false;
	PlayerController->StopOverrideInputMappingContext();
	
	if (!bIsPuzzleComplete)
	{
#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Clear sequence");
		}
#endif
		UE_LOG(LogG2I, Log, TEXT("%s: Clear sequence"), *GetActorNameOrLabel());
		IndexInCorrectSequence = 0;
		
		if (!ensure(SliderSM))
		{
			UE_LOG(LogG2I, Error, TEXT("Couldn't find SliderSM in %s"), *GetActorNameOrLabel());
		}
		else
		{
			SliderSM->SetRelativeLocation(SliderStartLocation);
		}
	}
	if (!ensure(HintKeyWidgetComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*UG2IWorldHintKeyWidgetComponent::StaticClass()->GetName());
	}
	else
	{
		HintKeyWidgetComp->SetIsLocked_Implementation(LauncherComp->IsLocked_Implementation());
	}
}

void AG2ISlider::FindAndSwitchLamp()
{
	if (CurrentCommonColorZone && !CurrentCommonColorZone->bIsActivationZone)
	{
		CurrentLamp = Lamps.FindRef(CurrentCommonColorZone->Color);
		if (CurrentLamp && CurrentLamp->DynamicMaterial)
		{
			if (CurrentLamp->GetBaseColor() != FColor::Transparent)
			{
				CurrentLamp->SetEmissiveColor(CurrentLamp->GetBaseColor());
			}
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
		const float SliderOffset = SliderSM->GetRelativeLocation().Y + MoveDir*CurrenImpulse*World->DeltaTimeSeconds;
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
