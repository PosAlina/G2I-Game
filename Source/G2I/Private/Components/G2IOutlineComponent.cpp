#include "Components/G2IOutlineComponent.h"
#include "G2I.h"
#include "G2IPlayerController.h"

void UG2IOutlineComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetupDefaults();
	BindDelegates();
}

void UG2IOutlineComponent::SetupDefaults()
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
		UE_LOG(LogG2I, Error, TEXT("Player controller doesn't exist in %s"), *GetName());
		return;
	}

	PlayerController = Cast<AG2IPlayerController>(LocalPlayerController);
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller isn't %s in %s"),
			*AG2IPlayerController::StaticClass()->GetName(), *GetName());
		return;
	}
}

void UG2IOutlineComponent::BindDelegates()
{
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller isn't %s in %s"),
			*AG2IPlayerController::StaticClass()->GetName(), *GetName());
		return;
	}
	PlayerController->OnPossessPawnDelegate.AddDynamic(this, &ThisClass::BindDelegatesByPawn);
	PlayerController->OnUnPossessPawnDelegate.AddDynamic(this, &ThisClass::UnBindDelegatesByPawn);
}

void UG2IOutlineComponent::BindDelegatesByPawn(APawn* Pawn)
{
	if (!Pawn)
	{
		return;
	}
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller isn't %s in %s"),
			*AG2IPlayerController::StaticClass()->GetName(), *GetName());
		return;
	}
	if (UG2IAimingComponent *AimingComponentByClass = PlayerController->GetAimingComponent())
	{
		AimingComponentByClass->OnStartAimingDelegate.AddDynamic(this, &ThisClass::StartOutline);
		AimingComponentByClass->OnFinishAimingDelegate.AddDynamic(this, &ThisClass::StopOutline);
	}
}

void UG2IOutlineComponent::UnBindDelegatesByPawn(APawn* Pawn)
{
	if (!Pawn)
	{
		return;
	}
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller isn't %s in %s"),
			*AG2IPlayerController::StaticClass()->GetName(), *GetName());
		return;
	}
	if (UG2IAimingComponent *AimingComponentByClass = PlayerController->GetAimingComponent())
	{
		AimingComponentByClass->OnStartAimingDelegate.RemoveDynamic(this, &ThisClass::StartOutline);
		AimingComponentByClass->OnFinishAimingDelegate.RemoveDynamic(this, &ThisClass::StopOutline);
	}
}

void UG2IOutlineComponent::StartOutline()
{
	OutlineController(true);
}

void UG2IOutlineComponent::StopOutline()
{
	OutlineController(false);
}

void UG2IOutlineComponent::OutlineController(const bool bOutlineMode) const
{
	const AActor* Parent = GetOwner();
	TArray<UStaticMeshComponent*> OutlineMeshes;

	if (!ensure(Parent))
	{
		UE_LOG(LogG2I, Error, TEXT("Component %s does not have parent"), *GetName());
		return;
	}
	
	Parent->GetComponents<UStaticMeshComponent>(OutlineMeshes);

	for (UStaticMeshComponent* OutlineMesh : OutlineMeshes)
	{
		if (!OutlineMesh)
		{
			continue;
		}
		
		if (bOutlineMode)
		{
			OutlineMesh->SetOverlayMaterial(OutlineMaterialInstance);
		}
		else
		{
			OutlineMesh->SetOverlayMaterial(nullptr);
		}
	}
}
