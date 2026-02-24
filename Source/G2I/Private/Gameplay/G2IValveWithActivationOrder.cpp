
#include "Gameplay/G2IValveWithActivationOrder.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "G2I.h"

AG2IValveWithActivationOrder::AG2IValveWithActivationOrder()
{
	ActivationOrderComponent = CreateDefaultSubobject<UG2IOwnerActivationOrderComponent>(TEXT("ActivationComponent"));
	ActivationOrderComponent->bCanBeReactivated = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetBoxExtent(FVector(60.0f, 50.0f, 50.0f));
	TriggerBox->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AG2IValveWithActivationOrder::OnTriggerBoxBeginOverlap);

	// i love tags so much
	// especially Interactive1 that every interactive object needs to have for some reason
	Tags.Add(FName("Interactive1"));
}

void AG2IValveWithActivationOrder::BeginPlay()
{
	Super::BeginPlay();

	bStartActivation = bActivated;
	if (bStartActivation)
	{
		ActivationsNum = 1;
		if (StaticMeshComponent)
			StaticMeshComponent->AddLocalRotation(MaxRotation);
	}
}

void AG2IValveWithActivationOrder::KeepActivation()
{
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

	if (CurrentRotation.Pitch > (MaxRotation.Pitch * (ActivationsNum + 1)) ||
		CurrentRotation.Roll > (MaxRotation.Roll * (ActivationsNum + 1)) ||
		CurrentRotation.Yaw > (MaxRotation.Yaw * (ActivationsNum + 1)))
		SetActorTickEnabled(false);

	if (CurrentRotation.Pitch < (MinRotation.Pitch + MaxRotation.Pitch * ActivationsNum) ||
		CurrentRotation.Roll < (MinRotation.Roll + MaxRotation.Roll * ActivationsNum) ||
		CurrentRotation.Yaw < (MinRotation.Yaw + MaxRotation.Yaw * ActivationsNum))
		SetActorTickEnabled(false);
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
		if (ActivationOrderComponent->bAccepted)
			ActivationsNum++;
		else
			ActivationsNum--;


		if (ActivationOrderComponent->bAccepted == bActivated)
			KeepActivation();
		else
			ChangeActivation();
	}
}

void AG2IValveWithActivationOrder::Deactivate_Implementation()
{
	// Restore valve start position
	if (ActivationsNum != 0)
	{
		if (bStartActivation)
			ActivationsNum = 1;
		else
			ActivationsNum = 0;

		if (bActivated)
			ChangeActivation();
		else
			KeepActivation();
	}

	bActivated = bStartActivation;
	if (ActivationOrderComponent)
		ActivationOrderComponent->bActivated = bStartActivation;
}
