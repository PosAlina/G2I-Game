#include "G2IPassThroughComponent.h"
#include "G2I.h"
#include "G2ICharacterCollisionComponent.h"
#include "G2INavAreaBlockPassableObjects.h"
#include "Components/G2ICharacterMovementComponent.h"
#include "G2IPlayerController.h"
#include "NavModifierComponent.h"

UG2IPassThroughComponent::UG2IPassThroughComponent()
{
	SetAreaClass(UG2INavAreaBlockPassableObjects::StaticClass());
}

void UG2IPassThroughComponent::OnRegister()
{
	Super::OnRegister();

	PreInitializeDefaults();
}

void UG2IPassThroughComponent::BeginPlay()
{
	Super::BeginPlay();

	BindingToDelegates();
}

void UG2IPassThroughComponent::BindingToDelegates()
{
	const UWorld* World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}

	APlayerController* FirstPlayerController = World->GetFirstPlayerController();
	if (!ensure(FirstPlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller doesn't exist in %s"), *GetName());
		return;
	}

	AG2IPlayerController* PlayerController = Cast<AG2IPlayerController>(FirstPlayerController);
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller isn't subclass of %s in %s"),
			*AG2IPlayerController::StaticClass()->GetName(), *GetName());
		return;
	}

	PlayerController->OnPossessPawnDelegate.AddDynamic(this,
		&UG2IPassThroughComponent::ShowGhostMaterial);
	PlayerController->OnUnPossessPawnDelegate.AddDynamic(this,
		&UG2IPassThroughComponent::HideGhostMaterial);
}

void UG2IPassThroughComponent::PreInitializeDefaults()
{
	Owner = GetOwner();
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
		return;
	}

	for (UActorComponent *ActorComponent : Owner->GetComponents())
	{
		if (ActorComponent->IsA<UNavModifierComponent>())
		{
			continue;
		}
		
		UPrimitiveComponent *Primitive = Cast<UPrimitiveComponent>(ActorComponent);
		if (Primitive && Primitive->IsCollisionEnabled())
		{
			Primitive->SetCollisionResponseToChannel(UG2ICharacterCollisionComponent::GetPassingThroughCollisionChannel(), ECollisionResponse::ECR_Ignore);
		}

		ActorComponent->SetCanEverAffectNavigation(false);
	}

	FVector Origin;
	Owner->GetActorBounds(true, Origin, FailsafeExtent, false);

	TInlineComponentArray<UStaticMeshComponent*> StaticMeshComponents;
	Owner->GetComponents<UStaticMeshComponent>(StaticMeshComponents, true);

	int32 SMCount = 0;
	for (UStaticMeshComponent* SMComponent : StaticMeshComponents)
	{
		StaticMeshesMap.Add(SMComponent, SMCount);

		for (auto* Material : SMComponent->GetMaterials())
		{
			MaterialsMap.Add(SMCount, Material);
		}

		// Enable rendering to custom depth buffer
		SMComponent->SetRenderCustomDepth(true);
		SMComponent->SetCustomDepthStencilValue(1);
		SMComponent->MarkRenderStateDirty();

		SMCount++;
	}

	UE_LOG(LogG2I, Log, TEXT("G2IPassThroughComponent: Found %i StaticMesh Components in %s"), SMCount, *Owner->GetActorNameOrLabel());
}

void UG2IPassThroughComponent::HideGhostMaterial(APawn* Pawn)
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s of %s"), *GetName(),
			*Owner->GetActorNameOrLabel());
		return;
	}

	if (!Pawn)
	{
		UE_LOG(LogG2I, Warning, TEXT("Attempting to change override material %s for nullptr pawn"), *Owner->GetActorNameOrLabel());
		return;
	}

	UG2ICharacterMovementComponent* Component = Cast<UG2ICharacterMovementComponent>(Pawn->GetMovementComponent());
	if (!ensure(Component))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner %s hasn't movement component in %s"), *Owner->GetName(), *GetName());
		return;
	}

	// Hiding ghost material if character can't pass through objects
	if (Component->CanPassThroughObject())
	{
		TArray<UMaterialInterface*> MaterialsArray;

		for (auto SMPair : StaticMeshesMap)
		{
			int32 SMCount = SMPair.Value;
			MaterialsMap.MultiFind(SMCount, MaterialsArray, false);

			for (int32 i = 0; i < MaterialsArray.Num(); i++)
				SMPair.Key->SetMaterial(i, MaterialsArray[i]);
		}
	}

	UE_LOG(LogG2I, Verbose, TEXT("G2IPassThroughComponent: Called HideGhostMaterial in %s"), *Owner->GetActorNameOrLabel());
}

void UG2IPassThroughComponent::ShowGhostMaterial(APawn* Pawn)
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s of %s"), *GetName(),
			*Owner->GetActorNameOrLabel());
		return;
	}

	if (!Pawn)
	{
		UE_LOG(LogG2I, Warning, TEXT("Attempting to change override material %s for nullptr pawn"), *Owner->GetActorNameOrLabel());
		return;
	}

	UG2ICharacterMovementComponent* Component = Cast<UG2ICharacterMovementComponent>(Pawn->GetMovementComponent());
	if (!ensure(Component))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner %s hasn't movement component in %s"), *Owner->GetName(), *GetName());
		return;
	}

	// Showing ghost material if character can pass through objects
	if (Component->CanPassThroughObject())
	{
		if (!ensure(GhostMaterial))
		{
			UE_LOG(LogG2I, Error, TEXT("Ghost Material isn't set up %s"), *GetName());
			return;
		}

		TArray<UMaterialInterface*> MaterialsArray;

		for (auto SMPair : StaticMeshesMap)
		{
			int32 SMCount = SMPair.Value;
			MaterialsMap.MultiFind(SMCount, MaterialsArray, false);

			for (int32 i = 0; i < MaterialsArray.Num(); i++)
				SMPair.Key->SetMaterial(i, GhostMaterial);
		}
	}

	UE_LOG(LogG2I, Verbose, TEXT("G2IPassThroughComponent: Called ShowGhostMaterial in %s"), *Owner->GetActorNameOrLabel());
}
