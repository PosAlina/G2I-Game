#include "G2IAIMovementComponent.h"
#include "AIController.h"
#include "G2I.h"
#include "G2ICharacterMovementComponent.h"
#include "G2INavigationFilterBlockPassableObjects.h"
#include "G2IPlayerController.h"
#include "Navigation/PathFollowingComponent.h"

UG2IAIMovementComponent::UG2IAIMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UG2IAIMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetupDefaults();
	BindDelegates();
}

void UG2IAIMovementComponent::SetupDefaults()
{
	SetupDefaultsAIOwnerController();
	SetupDefaultsPlayerController();
}

void UG2IAIMovementComponent::SetupDefaultsAIOwnerController()
{
	AActor *OwnerActor = GetOwner();
	if (!ensure(OwnerActor))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
		return;
	}

	OwnerAIController = Cast<AAIController>(OwnerActor);
	if (!ensure(OwnerAIController))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner %s isn't AI Controller in %s"),
			*OwnerActor->GetActorNameOrLabel(), *GetName());
		return;
	}
}

void UG2IAIMovementComponent::SetupDefaultsPlayerController()
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
		UE_LOG(LogG2I, Error, TEXT("Player Controller doesn't exist in %s"), *GetName());
		return;
	}

	bCanMove = PlayerController->bIsFollowingAIBehindPlayer;
}

void UG2IAIMovementComponent::BindDelegates()
{
	if (!ensure(OwnerAIController))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't identified as AI Controller in %s"), *GetName());
	}
	else
	{
		OwnerAIController->ReceiveMoveCompleted.AddDynamic(this, &ThisClass::MoveCompleted);
	}

	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller doesn't exist in %s"), *GetName());
	}
	else
	{
		PlayerController->OnToggleFollowAIBehindPlayerDelegate.AddDynamic(this, &ThisClass::SetCanMove);
	}
}

void UG2IAIMovementComponent::MoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	bIsMoving = false;
}

void UG2IAIMovementComponent::SetCanMove(bool bNewCanMove)
{
	bCanMove = bNewCanMove;
	if (!bCanMove)
	{
		StopMovement();
	}
}

void UG2IAIMovementComponent::UpdateNewAIPawn(APawn* NewAIPawn)
{
	FilterClass = UG2INavigationFilterBlockPassableObjects::StaticClass();
	if (const UG2ICharacterMovementComponent *CharacterMovementComponent = NewAIPawn->FindComponentByClass<UG2ICharacterMovementComponent>())
	{
		if (CharacterMovementComponent->CanPassThroughObject())
		{
			FilterClass = nullptr;
		}
	}
}

void UG2IAIMovementComponent::Activate_Implementation()
{
	bActiveComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UG2IAIMovementComponent::Deactivate_Implementation()
{
	PrimaryComponentTick.bCanEverTick = false;
	bActiveComponent = false;
	StopMovement();
}

void UG2IAIMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateTickMovement();
}

void UG2IAIMovementComponent::UpdateTickMovement()
{
	if (!OwnerAIController || !PlayerController)
	{
		return;
	}
	
	APawn *PlayerPawn = PlayerController->GetPawn();
	if (bActiveComponent && bCanMove && !bIsMoving && PlayerPawn && ShouldMove())
	{
		bIsMoving = true;
		OwnerAIController->MoveToActor(PlayerPawn, 100.f, true, true, true, FilterClass);
	}
}

void UG2IAIMovementComponent::StopMovement()
{
	OwnerAIController->StopMovement();
	bIsMoving = false;
}

bool UG2IAIMovementComponent::ShouldMove()
{
	if (!OwnerAIController || !PlayerController)
	{
		return false;
	}
	
	const APawn *CurrentAIPawn = OwnerAIController->GetPawn();
	const APawn *PlayerPawn = PlayerController->GetPawn();
	if (!PlayerPawn || !CurrentAIPawn)
	{
		return false;
	}
	
	const float PathDistance = FVector::Dist(CurrentAIPawn->GetActorLocation(), PlayerPawn->GetActorLocation());
	return PathDistance > 100.f;
}
