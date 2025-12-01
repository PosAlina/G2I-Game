#include "Components/G2IInteractionComponent.h"
#include "GameFramework/Character.h"
#include "G2IInteractiveObjectInterface.h"
#include "G2IMovingObjectInterface.h"
#include "Components/G2ICharacterMovementComponent.h"
#include <Components/BoxComponent.h>
#include <Components/CapsuleComponent.h>
#include "DrawDebugHelpers.h"
#include "G2I.h"

void UG2IInteractionComponent::BindingToDelegates()
{
	if (ACharacter* Owner = Cast<ACharacter>(GetOwner()))
	{
		if (UG2ICharacterMovementComponent* CharacterMovementComp = Owner->FindComponentByClass<UG2ICharacterMovementComponent>()) {
			CharacterMovementComp->OnJumpDelegate.AddDynamic(this, &UG2IInteractionComponent::HandleJumping);
			Owner->LandedDelegate.AddDynamic(this, &UG2IInteractionComponent::HandleLanded);
		}
	}
}

void UG2IInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	BindingToDelegates();
}

UG2IInteractionComponent::UG2IInteractionComponent()
{
	// Create a interaction box
	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void UG2IInteractionComponent::InteractAction_Implementation(const FName& Tag)
{
	if (!bCanInteract) {
		return;
	}
	AActor* Ow = GetOwner();
	if (Ow == nullptr)
	{
		UE_LOG(LogG2I, Log, TEXT("Component don't have owner"));
		return;
	}

	APawn* Pawn = Cast<APawn>(Ow);
	if (Pawn == nullptr)
	{
		UE_LOG(LogG2I, Log, TEXT("The component's owner is not a Pawn"));
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	if (PlayerController == nullptr)
	{
		UE_LOG(LogG2I, Log, TEXT("The component's owner is not controlled by the player"));
		return;
	}
	
	const ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (!Owner)
	{
		UE_LOG(LogG2I, Log, TEXT("The component's owner is not a Character"));
		return;
	}

	DrawDebugBox(
		GetWorld(),
		InteractionBox->GetComponentLocation(),
		InteractionBox->GetScaledBoxExtent(),
		InteractionBox->GetComponentQuat(),
		FColor::Green,
		false,
		1.0f,
		0,
		2.0f
	);

	TArray<AActor*> OverlappedActors;
	InteractionBox->GetOverlappingActors(OverlappedActors);

	for (AActor* Overlap : OverlappedActors)
	{
		if (!Overlap)
		{
			UE_LOG(LogG2I, Log, TEXT("Overlap actor is null!"));
			continue;
		}

		if (Overlap->ActorHasTag(Tag))
		{

			if (Overlap->Implements<UG2IInteractiveObjectInterface>())
			{

				if (IG2IInteractiveObjectInterface::Execute_CanInteract(Overlap, Owner))
				{
					if (Overlap->Implements<UG2IMovingObjectInterface>()) {
						float SpeedChange = IG2IMovingObjectInterface::Execute_GetSpeedChange(Overlap);
						OnMovingInteractingDelegate.Broadcast(SpeedChange);
					}
					IG2IInteractiveObjectInterface::Execute_Interact(Overlap, Owner);
				}
				else
				{
					UE_LOG(LogG2I, Log, TEXT("Actor %s can't interact"), *Overlap->GetName());
				}
			}
			else
			{
				UE_LOG(LogG2I, Log, TEXT("Actor %s doesn't implement Interactive Object Interface"), *Overlap->GetName());
			}
		}
		else
		{
			UE_LOG(LogG2I, Log, TEXT("Actor %s doesn't have tag %s"), *Overlap->GetName(), *Tag.ToString());
		}
	}
}

void UG2IInteractionComponent::HandleJumping()
{
	bCanInteract = false;
}

void UG2IInteractionComponent::HandleLanded(const FHitResult& Hit)
{
	bCanInteract = true;
}

void UG2IInteractionComponent::OnRegister()
{
	Super::OnRegister();

	if (AActor* Owner = GetOwner())
	{
		if (USceneComponent* Root = Owner->GetRootComponent())
		{
			InteractionBox->AttachToComponent(
				Root,
				FAttachmentTransformRules::SnapToTargetNotIncludingScale
			);
			if (ACharacter* Character = Cast<ACharacter>(Owner))
			{
				UCapsuleComponent* Capsule = Character->GetCapsuleComponent();
				if (Capsule)
				{
					float Radius = Capsule->GetUnscaledCapsuleRadius();
					float HalfHeight = Capsule->GetUnscaledCapsuleHalfHeight();;

					float Length = InteractionBoxLength;

					FVector HalfSize;
					HalfSize.X = Length * 0.5f;
					HalfSize.Y = Radius; 
					HalfSize.Z = HalfHeight;

					InteractionBox->SetBoxExtent(HalfSize);

					InteractionBox->SetRelativeLocation(
						FVector(Length * 0.5f + Radius, 0.f, 0.f)
					);
				}
				else {
					UE_LOG(LogG2I, Log, TEXT("Character don't have capsule"));
				}
			}
			else {
				UE_LOG(LogG2I, Log, TEXT("Owner isn't a Character"));
				return;
			}
		}
		else {
			UE_LOG(LogG2I, Log, TEXT("Owner don't have RootComponent"));
		}
	}
	else {
		UE_LOG(LogG2I, Log, TEXT("Component don't have owner"));
	}
}
