#include "Components/G2IInteractionComponent.h"
#include "GameFramework/Character.h"
#include "G2IInteractiveObjectInterface.h"
#include "G2IMovingObjectInterface.h"
#include "Components/G2ICharacterMovementComponent.h"
#include <Components/BoxComponent.h>
#include <Components/CapsuleComponent.h>
#include "DrawDebugHelpers.h"
#include "G2I.h"
#include "G2IGameInstance.h"
#include "G2IPlayerController.h"
#include "Public/UI/WidgetComponents/G2IWorldHintKeyWidgetComponent.h"

void UG2IInteractionComponent::BindingToDelegates()
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner is nullptr in %s"), *GetName());
		return;
	}
	
	if (UG2ICharacterMovementComponent* CharacterMovementComp = Owner->FindComponentByClass<UG2ICharacterMovementComponent>()) {
		CharacterMovementComp->OnJumpDelegate.AddDynamic(this, &UG2IInteractionComponent::HandleJumping);
		Owner->LandedDelegate.AddDynamic(this, &UG2IInteractionComponent::HandleLanded);
	}

	if (!ensure(InteractionBox))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't have interaction box component"), *GetName());
		return;
	}
	InteractionBox->OnComponentBeginOverlap.AddDynamic(this, &UG2IInteractionComponent::OnInteractionBoxBeginOverlap);
	InteractionBox->OnComponentEndOverlap.AddDynamic(this, &UG2IInteractionComponent::OnInteractionBoxEndOverlap);
}

void UG2IInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupDefaults();
	BindingToDelegates();

	TArray<AActor*> OverlappingActors;
	InteractionBox->GetOverlappingActors(OverlappingActors);
	for (AActor* OverlappingActor : OverlappingActors)
	{
		OpenKeyHintByActor(OverlappingActor);
	}
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
	
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}
	if (!ensure(InteractionBox))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't have interaction box component"), *GetName());
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
				if (UG2IWorldHintKeyWidgetComponent *KeyHintComponent =
					IG2IInteractiveObjectInterface::Execute_GetInteractionKeyHintComponent(Overlap))
				{
					if (!ensure(UIManager))
					{
						UE_LOG(LogG2I, Warning, TEXT("%s isn't defined in %s"),
							*UG2IUIManager::StaticClass()->GetName(), *GetName());
					}
					else
					{
						if (!UIManager->CanSeeWorldWidget(KeyHintComponent))
						{
							UE_LOG(LogG2I, Log, TEXT("%s can't interact with %s, because it can't see key hint %s"),
								*Owner->GetActorNameOrLabel(), *Overlap->GetActorNameOrLabel(),
								*KeyHintComponent->GetName());
							continue;
						}
					}
				}
				
				if (IG2IInteractiveObjectInterface::Execute_CanInteract(Overlap, Owner))
				{
					if (Overlap->Implements<UG2IMovingObjectInterface>()) {
						float SpeedChange = IG2IMovingObjectInterface::Execute_GetSpeedChange(Overlap);
						OnMovingInteractingDelegate.Broadcast(SpeedChange);
					}
					IG2IInteractiveObjectInterface::Execute_Interact(Overlap, Owner);
					const FString InteractLogMessage = "Player interacted with " + Overlap->GetActorNameOrLabel();
#if WITH_EDITOR
					if (GEngine)
					{
						GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, InteractLogMessage);
					}
#endif
					UE_LOG(LogG2I, Log, TEXT("%s"), *InteractLogMessage);
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

	AActor *OwnerActor = GetOwner();
	if (!ensure(OwnerActor))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
		return;
	}
	Owner = Cast<ACharacter>(OwnerActor);
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s of %s"), *GetName(),
			*OwnerActor->GetActorNameOrLabel());
		return;
	}
	if (!ensure(InteractionBox))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't have interaction box component"), *GetName());
		return;
	}
	
	if (USceneComponent* Root = OwnerActor->GetRootComponent())
	{
		InteractionBox->AttachToComponent(
			Root,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale
		);
		UCapsuleComponent* Capsule = Owner->GetCapsuleComponent();
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
			UE_LOG(LogG2I, Log, TEXT("Character doesn't have a capsule component"));
		}
	}
	else {
		UE_LOG(LogG2I, Log, TEXT("Owner doesn't have a RootComponent"));
	}
}

void UG2IInteractionComponent::SetupDefaults()
{
	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}

	APlayerController *LocalPlayerController = World->GetFirstPlayerController();
	if (!ensure(LocalPlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Local player controller doesn't exist in %s"), *GetName());
		return;
	}

	PlayerController = Cast<AG2IPlayerController>(LocalPlayerController);
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller %s isn't %s in %s"),
			*LocalPlayerController->GetActorNameOrLabel(), *AG2IPlayerController::StaticClass()->GetName(), *GetName());
		return;
	}

	SetTagOfInteractionActions();
	
	const UG2IGameInstance *GameInstance = Cast<UG2IGameInstance>(World->GetGameInstance());
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("Game Instance doesn't exist in %s"), *GetName());
		return;
	}
	UIManager = GameInstance->GetSubsystem<UG2IUIManager>();
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s isn't defined in %s"),
			*UG2IUIManager::StaticClass()->GetName(), *GetName());
		return;
	}
}

void UG2IInteractionComponent::SetTagOfInteractionActions()
{
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller doesn't exist in %s"), *GetName());
		return;
	}
	TagOfInteractionActions = PlayerController->GetActionToTagMap();
}

void UG2IInteractionComponent::OnInteractionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	OpenKeyHintByActor(OtherActor);
}

void UG2IInteractionComponent::OnInteractionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CloseKeyHintByActor(OtherActor);
}

void UG2IInteractionComponent::OpenKeyHintByActor(AActor* OtherActor)
{
	if (OtherActor && OtherActor->Implements<UG2IInteractiveObjectInterface>())
	{
		if (UG2IWorldHintKeyWidgetComponent *KeyHintComponent =
		IG2IInteractiveObjectInterface::Execute_GetInteractionKeyHintComponent(OtherActor))
		{
			if (IG2IInteractiveObjectInterface::Execute_CanInteract(OtherActor, Owner))
			{
				TArray<UInputAction *> InteractionActions;
				for (auto& [Action, Tag] : TagOfInteractionActions)
				{
					if (OtherActor->ActorHasTag(Tag))
					{
						InteractionActions.Add(Action);
					}
				}
				if (!InteractionActions.IsEmpty())
				{
					// TODO: add support for multiple widgets with different keys
					KeyHintComponent->OpenKeyHint(InteractionActions[0]);
				}
			}
		}
	}
}

void UG2IInteractionComponent::CloseKeyHintByActor(AActor* OtherActor)
{
	if (OtherActor && OtherActor->Implements<UG2IInteractiveObjectInterface>())
	{
		if (UG2IWorldHintKeyWidgetComponent *KeyHintComponent =
		IG2IInteractiveObjectInterface::Execute_GetInteractionKeyHintComponent(OtherActor))
		{
			KeyHintComponent->CloseKeyHint();
		}
	}
}
