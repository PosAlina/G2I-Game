#include "Components/G2IInteractionComponent.h"
#include "GameFramework/Character.h"
#include "G2IInteractiveObjectInterface.h"
#include <Components/SphereComponent.h>


UG2IInteractionComponent::UG2IInteractionComponent()
{
	// Create a interaction sphere
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->InitSphereRadius(InteractionSphereRadius);
	InteractionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void UG2IInteractionComponent::InteractAction_Implementation(const FName& Tag)
{
	AActor* Ow = GetOwner();
	if (Ow == nullptr)
	{
		return;
	}

	APawn* Pawn = Cast<APawn>(Ow);
	if (Pawn == nullptr)
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	if (PlayerController == nullptr)
	{
		return;
	}
	
	const ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (!Owner)
	{
		return;
	}

	DrawDebugSphere(
			GetWorld(),
			InteractionSphere->GetComponentLocation(),
			InteractionSphere->GetScaledSphereRadius(),
			16,
			FColor::Green,
			false,
			1.0f,
			0,
			2.0f
			);

	TArray<AActor*> OverlappedActors;
	InteractionSphere->GetOverlappingActors(OverlappedActors);

	UE_LOG(LogTemp, Log, TEXT("InteractAction: found %d overlapping actors"), OverlappedActors.Num());

	for (AActor* Overlap : OverlappedActors)
	{
		if (!Overlap)
		{
			UE_LOG(LogTemp, Log, TEXT("Overlap actor is null!"));
			continue;
		}

		UE_LOG(LogTemp, Log, TEXT("Checking actor: %s"), *Overlap->GetName());

		if (Overlap->ActorHasTag(Tag))
		{
			UE_LOG(LogTemp, Log, TEXT("Actor %s has tag %s"), *Overlap->GetName(), *Tag.ToString());

			if (Overlap->Implements<UG2IInteractiveObjectInterface>())
			{
				UE_LOG(LogTemp, Log, TEXT("Actor %s implements IG2IInteractiveObjectInterface"), *Overlap->GetName());

					if (IG2IInteractiveObjectInterface::Execute_CanInteract(Overlap, Owner))
					{
						UE_LOG(LogTemp, Log, TEXT("Actor %s CAN interact, executing Interact"), *Overlap->GetName());
						IG2IInteractiveObjectInterface::Execute_Interact(Overlap, Owner);
					}
					else
					{
						UE_LOG(LogTemp, Log, TEXT("Actor %s CANNOT interact"), *Overlap->GetName());
					}
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Actor %s does NOT implement IG2IInteractiveObjectInterface"), *Overlap->GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Actor %s does NOT have tag %s"), *Overlap->GetName(), *Tag.ToString());
		}
	}
}

void UG2IInteractionComponent::OnRegister()
{
	Super::OnRegister();

	if (AActor* Owner = GetOwner())
	{
		if (USceneComponent* Root = Owner->GetRootComponent())
		{
			InteractionSphere->AttachToComponent(
				Root,
				FAttachmentTransformRules::SnapToTargetNotIncludingScale
			);
		}
	}
}
