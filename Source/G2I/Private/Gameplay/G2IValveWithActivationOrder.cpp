
#include "Gameplay/G2IValveWithActivationOrder.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "G2I.h"
#include "Sound/G2ISoundComponent.h"

AG2IValveWithActivationOrder::AG2IValveWithActivationOrder()
{
	ActivationOrderComponent = CreateDefaultSubobject<UG2IOwnerActivationOrderComponent>(TEXT("ActivationComponent"));
	if (ActivationOrderComponent)
	{
		ActivationOrderComponent->bCanBeReactivated = true;
	}

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	if (TriggerBox)
	{
		TriggerBox->SetBoxExtent(FVector(60.0f, 50.0f, 50.0f));
		TriggerBox->SetupAttachment(GetRootComponent());
	}

	Tags.Add(FName("Interactive1"));
}

void AG2IValveWithActivationOrder::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerBox)
	{
		TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AG2IValveWithActivationOrder::OnTriggerBoxBeginOverlap);
	}

	if (IsDeltaNegative())
	{
		MinMultiplier = -1;
		DiffRotation = MaxRotation - MinRotation;
	}
	else
	{
		MaxMultiplier = -1;
		DiffRotation = MinRotation - MaxRotation;
	}

	bStartActivation = bActivated;
	if (bStartActivation)
	{
		ActivationsNum = 1;
		if (StaticMeshComponent)
		{
			if (IsDeltaNegative())
			{
				StaticMeshComponent->AddLocalRotation(MinRotation);
			}
			else
			{
				StaticMeshComponent->AddLocalRotation(MaxRotation);
			}
		}
	}
}

void AG2IValveWithActivationOrder::KeepActivation()
{
	if (SoundComp)
	{
		SoundComp->PlaySound(ValveRotationSoundId);
	}

	if (StaticMeshComponent)
		SetActorTickEnabled(true);
}

void AG2IValveWithActivationOrder::OnTriggerBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (auto* OtherCharacter = Cast<ACharacter>(OtherActor))
		if (AllowedCharactersToOverlapSet.Contains(OtherCharacter) && OtherCharacter->IsPlayerControlled() && OtherComp && OtherComp->IsA<UCapsuleComponent>())
		{
			if (SoundOnOverlap)
				UGameplayStatics::PlaySoundAtLocation(this, SoundOnOverlap, GetActorLocation());
		}
}

void AG2IValveWithActivationOrder::ApplyLocalRotation()
{
	StaticMeshComponent->AddLocalRotation(DeltaRotation);
	CurrentRotation += DeltaRotation;

	if (CurrentRotation.Pitch > TargetMaxRotation.Pitch ||
		CurrentRotation.Roll > TargetMaxRotation.Roll ||
		CurrentRotation.Yaw > TargetMaxRotation.Yaw)
	{
		StopLocalRotation();
	}

	if (CurrentRotation.Pitch < TargetMinRotation.Pitch ||
		CurrentRotation.Roll < TargetMinRotation.Roll ||
		CurrentRotation.Yaw < TargetMinRotation.Yaw)
	{
		StopLocalRotation();
	}
}

void AG2IValveWithActivationOrder::Interact_Implementation(const ACharacter* Interactor)
{
	if (ActivationOrderComponent)
		ActivationOrderComponent->Activated();
}

void AG2IValveWithActivationOrder::Activate_Implementation()
{
	if (ActivationOrderComponent)
	{
		const bool bAcceptedByManager = ActivationOrderComponent->bAccepted;
		if (bAcceptedByManager)
			ActivationsNum++;
		else
			ActivationsNum--;

		TargetMinRotation = DiffRotation * (MinMultiplier + ActivationsNum + !bAcceptedByManager);
		TargetMaxRotation = DiffRotation * (MaxMultiplier + ActivationsNum + !bAcceptedByManager);

		if (bAcceptedByManager == bActivated)
			KeepActivation();
		else
			ChangeActivation();
	}
}

void AG2IValveWithActivationOrder::Deactivate_Implementation()
{
	if (IsActorTickEnabled() && GetWorld())
	{
		GetWorldTimerManager().SetTimer(DeactivationTimer, this, &ThisClass::RestorePosition, DelayTimeToDeactivate, false);
		return;
	}

	RestorePosition();
}

void AG2IValveWithActivationOrder::RestorePosition()
{
	if (ActivationsNum != 0)
	{
		if (bStartActivation)
			ActivationsNum = 1;
		else
			ActivationsNum = 0;

		if (bActivated != bStartActivation)
		{
			TargetMinRotation = DiffRotation * (MinMultiplier + ActivationsNum + bActivated);
			TargetMaxRotation = DiffRotation * (MaxMultiplier + ActivationsNum + bActivated);
			ChangeActivation();
		}
		else
		{
			TargetMinRotation = DiffRotation * (MinMultiplier + ActivationsNum + !bActivated);
			TargetMaxRotation = DiffRotation * (MaxMultiplier + ActivationsNum + !bActivated);
			KeepActivation();
		}
	}

	if (ActivationOrderComponent)
		ActivationOrderComponent->bActivated = bStartActivation;
}

void AG2IValveWithActivationOrder::StopLocalRotation()
{
	CurrentRotation = (MaxRotation + MinRotation) * ActivationsNum;

	Super::StopLocalRotation();

	if (GetWorld() && GetWorldTimerManager().IsTimerActive(DeactivationTimer))
	{
		GetWorldTimerManager().ClearTimer(DeactivationTimer);
		RestorePosition();
	}
}

bool AG2IValveWithActivationOrder::IsDeltaNegative() const
{
	if (DeltaRotation.Pitch < 0. ||
		DeltaRotation.Yaw < 0. ||
		DeltaRotation.Roll < 0.)
	{
		return true;
	}

	return false;
}
