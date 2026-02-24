
#include "Gameplay/G2IValveWithActivationOrder.h"
#include "G2I.h"

AG2IValveWithActivationOrder::AG2IValveWithActivationOrder()
{
	ActivationOrderSMeshComponent = CreateDefaultSubobject<UG2IOwnerActivationOrderComponent>(TEXT("ActivationComponent"));
}

void AG2IValveWithActivationOrder::BeginPlay()
{
	Super::BeginPlay();

	bStartActivation = bActivated;
}

void AG2IValveWithActivationOrder::KeepActivation()
{
	if (StaticMeshComponent)
		SetActorTickEnabled(true);
}

void AG2IValveWithActivationOrder::ApplyLocalRotation()
{
	StaticMeshComponent->AddLocalRotation(DeltaRotation);
	CurrentRotation += DeltaRotation;

	if (CurrentRotation.Pitch > MaxRotation.Pitch * ActivationsNum ||
		CurrentRotation.Roll > MaxRotation.Roll * ActivationsNum ||
		CurrentRotation.Yaw > MaxRotation.Yaw * ActivationsNum)
		SetActorTickEnabled(false);

	if (CurrentRotation.Pitch < MinRotation.Pitch + MaxRotation.Pitch * ActivationsNum ||
		CurrentRotation.Roll < MinRotation.Roll + MaxRotation.Roll * ActivationsNum ||
		CurrentRotation.Yaw < MinRotation.Yaw + MaxRotation.Yaw * ActivationsNum)
		SetActorTickEnabled(false);
}

void AG2IValveWithActivationOrder::Interact_Implementation(const ACharacter* Interactor)
{
	if (ActivationOrderSMeshComponent)
		ActivationOrderSMeshComponent->Activated();
}

void AG2IValveWithActivationOrder::Activate_Implementation()
{
	if (ActivationOrderSMeshComponent)
	{
		if (ActivationOrderSMeshComponent->bAccepted)
			ActivationsNum++;
		else
			ActivationsNum--;


		if (ActivationOrderSMeshComponent->bAccepted == bActivated)
			KeepActivation();
		else
			ChangeActivation();
	}
}

void AG2IValveWithActivationOrder::Deactivate_Implementation()
{
	ActivationsNum = 0;
	if (bStartActivation != bActivated)
		ChangeActivation();
}
