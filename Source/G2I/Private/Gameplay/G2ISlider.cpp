


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

	SetRootComponent(SliderBaseSM);
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
		EnhancedInputComponent->BindAction(MoveSliderAction, ETriggerEvent::Completed, this, &ThisClass::MoveSliderInertia);
		EnhancedInputComponent->BindAction(SliderExitAction, ETriggerEvent::Started, this, &ThisClass::SliderExit);
	}
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
	return true;
}

void AG2ISlider::OnSliderBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto CurrentColorZoneCol = Cast<UBoxComponent>(OtherComp);

	if (!CurrentColorZoneCol)
    {
		UE_LOG(LogTemp, Log, TEXT("Not CurrentColorZoneCol"));
    	return;
    }
	
	auto CurrentColorZone = Cast<UG2IColorZoneComponent>(CurrentColorZoneCol->GetAttachParent());

	if (!CurrentColorZone)
	{
		UE_LOG(LogTemp, Log, TEXT("CurrentColorZoneCol hasnt ColorZone"));
		return;
	}

	bool bIsZoneColorEqualColorInSequence = CompareZoneColorToColorInSequence(*CurrentColorZone);

	if (!bIsZoneColorEqualColorInSequence)
	{
		IndexInCorrectSequence = 0;
		CompareZoneColorToColorInSequence(*CurrentColorZone);
	}

	FindAndSwitchLamp(true, *CurrentColorZone);
}

void AG2ISlider::OnSliderEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto CurrentColorZoneCol = Cast<UBoxComponent>(OtherComp);

	if (!CurrentColorZoneCol)
	{
		UE_LOG(LogTemp, Log, TEXT("Not CurrentColorZoneCol"));
		return;
	}
	
	auto CurrentColorZone = Cast<UG2IColorZoneComponent>(CurrentColorZoneCol->GetAttachParent());

	if (!CurrentColorZone)
	{
		UE_LOG(LogTemp, Log, TEXT("CurrentColorZoneCol hasnt ColorZone"));
		return;
	}

	FindAndSwitchLamp(false, *CurrentColorZone);
}

bool AG2ISlider::CompareZoneColorToColorInSequence(UG2IColorZoneComponent& CurrentColorZone)
{
	if (!bIsPuzzleComplete && CorrectSequence[IndexInCorrectSequence] == CurrentColorZone.Color)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, "Correct");
		IndexInCorrectSequence++;
		if (IndexInCorrectSequence == CorrectSequence.Num())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Correct All");
			bIsPuzzleComplete = true;
		}
		return true;
	}
	return false;
}


void AG2ISlider::MoveSlider(const FInputActionValue& Value)
{
	if (bIsSliderActive)
	{
		float MoveDir = Value.Get<float>();
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

void AG2ISlider::MoveSliderInertia(const FInputActionValue& Value)
{

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

void AG2ISlider::FindAndSwitchLamp(bool bLampMode, UG2IColorZoneComponent& CurrentColorZone)
{
	auto ActorComponents = GetComponents();

	if (ActorComponents.Num() > 0)
	{
		for (auto Comp : ActorComponents)
		{
			auto LampComp = Cast<UG2ISliderLampComponent>(Comp);
			if (LampComp && LampComp->Color == CurrentColorZone.Color)
			{
				LampComp->LampLight->SetVisibility(bLampMode);
				if (bLampMode)
				{
					LampComp->LampMesh->SetMaterial(0, LampComp->LightOnMaterial);
				}
				else
				{
					LampComp->LampMesh->SetMaterial(0, LampComp->LightOffMaterial);
				}
				break;
			}
		}
	}
}