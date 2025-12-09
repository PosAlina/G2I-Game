
#include "Gameplay/G2IPipe.h"
#include "G2I.h"
#include "Components/ArrowComponent.h"

// Sets default values
AG2IPipe::AG2IPipe()
{
	SplineMetadata = CreateDefaultSubobject<UG2IPipesSplineMetadata>(TEXT("SplineMetadata"));

	SplineComponent = CreateDefaultSubobject<UG2IPipesSplineComponent>(TEXT("SplineComponent"));
	if (SplineComponent)
		SetRootComponent(SplineComponent);
}

void AG2IPipe::OnConstruction(const FTransform& Transform)
{
	//  *** Remove when custom spline metadata gets fixed ***
	if (PointParams.Num() < SplineComponent->GetNumberOfSplinePoints())
	{
		int diff = SplineComponent->GetNumberOfSplinePoints() - PointParams.Num();
		PointParams.Reserve(SplineComponent->GetNumberOfSplinePoints());
		for (int i = 0; i < diff; i++)
			PointParams.AddDefaulted();
	}
	//  *** End of the custom metadata crutch ***

	if (!DefaultMesh)
	{
		UE_LOG(LogG2I, Error, TEXT("Default Mesh isn't set in %s"), *GetName());
		return;
	}

	UStaticMesh* Mesh = nullptr;
	bCanAirPassThrough = true;

	for (int PointIndex = 0; PointIndex < SplineComponent->GetNumberOfSplinePoints(); PointIndex++)
	{
		// Setting Up Static Mesh & Interactable Objects

		// *	Spawn Collision Box if we Can Interact
		if (GetCanInteractAtSplinePoint(PointIndex))
			SpawnInteractableBoxComponent(PointIndex);

		// *	If no pipe here - skip SM generation part
		if (!GetHasPipeAtSplinePoint(PointIndex))
		{
			bCanAirPassThrough = false;
			continue;
		}

		// *	Holes / Valves Checks
		if (GetHasTechnicalHoleAtSplinePoint(PointIndex))
		{
			if (BrokenMesh)
			{
				bCanAirPassThrough = false;
				SpawnHoleActor(PointIndex);
				Mesh = BrokenMesh;
			}
			else
				UE_LOG(LogG2I, Error, TEXT("Broken Mesh isn't set in %s"), *GetName());
		}
		else
			Mesh = DefaultMesh;
		if (GetHasValveAtSplinePoint(PointIndex))
			SpawnValveActor(PointIndex);
		
		// Pipes Connections
		if (GetSendToOtherPipeAtSplinePoint(PointIndex))
		{
			UG2IPipesBoxComponent* box = SpawnPipesBoxComponent(PointIndex, false);
			sendingBoxComponents.Add(box);
		}
		if (GetResieveFromOtherPipeAtSplinePoint(PointIndex))
			SpawnPipesBoxComponent(PointIndex, true);

		// Generating Mesh
		if (PointIndex < (SplineComponent->GetNumberOfSplinePoints() - 1))
			GenerateMesh(Mesh, PointIndex);
	}
}

void AG2IPipe::BeginPlay()
{
	// Force call overlap events
	for (UG2IPipesBoxComponent* box : sendingBoxComponents)
	{
		TArray<AActor*> OverlappingActors;
		box->GetOverlappingActors(OverlappingActors);

		for (AActor* otherActor : OverlappingActors)
			OnPipeBeginOverlap(box, otherActor, nullptr, 0, false, FHitResult());
	}

	SendAir();
}

void AG2IPipe::RecieveAir_Implementation(AActor* sender, bool bAirPassed)
{
	if (ActorsToSendAirTo.Contains(sender))
	{
		UE_LOG(LogG2I, Warning, TEXT("Loop detected in %s!"), *GetName());
		return;
	}

	bHasAirPassed = bAirPassed;
	SendAir();
}

void AG2IPipe::OnPipeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("Overlap in %s Actor!"), *GetName()));

	if (UG2IPipesBoxComponent* box = Cast<UG2IPipesBoxComponent>(OverlappedComp))
	{
		if (box->bRecieves)
		{
			AddActorToSendAirTo(box->Owner);
		}
	}
}

void AG2IPipe::OnPipeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("End overlap in %s Actor"), *GetName()));

	if (UG2IPipesBoxComponent* box = Cast<UG2IPipesBoxComponent>(OverlappedComp))
	{
		if (box->bRecieves)
		{
			RemoveActorFromSendAirTo(box->Owner);
		}
	}
}

TObjectPtr<UG2IPipesSplineMetadata> AG2IPipe::GetSplineMetadata() const
{
	return SplineMetadata;
}

bool AG2IPipe::GetHasAirPassed() const
{
	return bHasAirPassed;
}

void AG2IPipe::SetHasAirPassed(bool newAirPassed)
{
	bHasAirPassed = newAirPassed;
}

bool AG2IPipe::GetAir() const
{
	return bHasAirPassed && bCanAirPassThrough;
}

float AG2IPipe::GetTestFloatAtSplinePoint(int32 PointIndex)
{
	if (ensure(SplineMetadata))
	{
		if (ensure(SplineMetadata->PointParams.IsValidIndex(PointIndex)))
		{
			return SplineMetadata->PointParams[PointIndex].TestValue;
		}
	}

	return 0.0f;
}

bool AG2IPipe::GetHasPipeAtSplinePoint(int32 PointIndex)
{
	/*if (ensure(SplineMetadata))
	{
		if (ensure(SplineMetadata->PointParams.IsValidIndex(PointIndex)))
		{
			return SplineMetadata->PointParams[PointIndex].bHasPipe;
		}
	}*/

	if (ensure(PointParams.IsValidIndex(PointIndex)))
	{
		return PointParams[PointIndex].bHasPipe;
	}

	return false;
}

bool AG2IPipe::GetHasValveAtSplinePoint(int32 PointIndex)
{
	/*if (ensure(SplineMetadata))
	{
		if (ensure(SplineMetadata->PointParams.IsValidIndex(PointIndex)))
		{
			return SplineMetadata->PointParams[PointIndex].bHasValve;
		}
	}*/

	if (ensure(PointParams.IsValidIndex(PointIndex)))
	{
		return PointParams[PointIndex].bHasValve;
	}

	return false;
}

bool AG2IPipe::GetHasTechnicalHoleAtSplinePoint(int32 PointIndex)
{
	/*if (ensure(SplineMetadata))
	{
		if (ensure(SplineMetadata->PointParams.IsValidIndex(PointIndex)))
		{
			return SplineMetadata->PointParams[PointIndex].bHasTechnicalHole;
		}
	}*/

	if (ensure(PointParams.IsValidIndex(PointIndex)))
	{
		return PointParams[PointIndex].bHasTechnicalHole;
	}

	return false;
}

bool AG2IPipe::GetCanInteractAtSplinePoint(int32 PointIndex)
{
	/*if (ensure(SplineMetadata))
	{
		if (ensure(SplineMetadata->PointParams.IsValidIndex(PointIndex)))
		{
			return SplineMetadata->PointParams[PointIndex].bCanInteractAtSplinePoint;
		}
	}*/

	if (ensure(PointParams.IsValidIndex(PointIndex)))
	{
		return PointParams[PointIndex].bCanInteractAtSplinePoint;
	}

	return false;
}

bool AG2IPipe::GetSendToOtherPipeAtSplinePoint(int32 PointIndex)
{
	if (ensure(PointParams.IsValidIndex(PointIndex)))
	{
		return PointParams[PointIndex].bSendToOtherPipe;
	}

	return false;
}

bool AG2IPipe::GetResieveFromOtherPipeAtSplinePoint(int32 PointIndex)
{
	if (ensure(PointParams.IsValidIndex(PointIndex)))
	{
		return PointParams[PointIndex].bResieveFromOtherPipe;
	}

	return false;
}

UG2IPipesBoxComponent* AG2IPipe::SpawnPipesBoxComponent(int32 PointIndex, bool bRecieves)
{
	UG2IPipesBoxComponent* collisionBox = (UG2IPipesBoxComponent*)(AddComponentByClass(UG2IPipesBoxComponent::StaticClass(), false, SplineComponent->GetTransformAtSplinePoint(PointIndex, ESplineCoordinateSpace::Local), false));
	collisionBox->SetBoxExtent(FVector(CollisionBoxExtent));
	collisionBox->bRecieves = bRecieves;
	collisionBox->Owner = this;
	collisionBox->PointIndex = PointIndex;
	collisionBox->SetGenerateOverlapEvents(true);
	collisionBox->SetCollisionObjectType(ECC_GameTraceChannel3);					 // Pipes Custom Collision
	collisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap); // Pipes Custom Collision
	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &AG2IPipe::OnPipeBeginOverlap);
	collisionBox->OnComponentEndOverlap.AddDynamic(this, &AG2IPipe::OnPipeEndOverlap);

	return collisionBox;
}

void AG2IPipe::SpawnHoleActor(int32 PointIndex)
{
	if (!HoleActor)
	{
		UE_LOG(LogG2I, Error, TEXT("Hole Actor isn't set in %s"), *GetName());
		return;
	}

	// Define Spawn Parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this; // Set the owner
	SpawnParams.Instigator = GetInstigator(); // Set the instigator
	SpawnParams.bNoFail = true; // Don't fail if collision fails
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // How to handle overlaps

	// Get Spawn Location & Rotation
	FVector SpawnLocation = GetLocationBetweenPoints(PointIndex, PointIndex + 1);
	FRotator SpawnRotation = SplineComponent->GetUpVectorAtSplinePoint(PointIndex, ESplineCoordinateSpace::Local).Rotation();

	// Spawn the Actor
	AActor* Hole = GetWorld()->SpawnActor<AActor>(
		HoleActor->StaticClass(),
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	if (!Hole)
		UE_LOG(LogG2I, Error, TEXT("Hole spawn failed."));
}

void AG2IPipe::SpawnValveActor(int32 PointIndex)
{
	if (!ValveActor)
	{
		UE_LOG(LogG2I, Error, TEXT("Valve Actor isn't set in %s"), *GetName());
		return;
	}

	// Define Spawn Parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this; // Set the owner
	SpawnParams.Instigator = GetInstigator(); // Set the instigator
	SpawnParams.bNoFail = true; // Don't fail if collision fails
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // How to handle overlaps

	// Get Spawn Location & Rotation
	FVector SpawnLocation = GetLocationBetweenPoints(PointIndex, PointIndex + 1);
	FRotator SpawnRotation = SplineComponent->GetUpVectorAtSplinePoint(PointIndex, ESplineCoordinateSpace::Local).Rotation();

	// Spawn the Actor
	AActor* Valve = GetWorld()->SpawnActor<AActor>(
		ValveActor->StaticClass(),
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	if (!Valve)
		UE_LOG(LogG2I, Error, TEXT("Valve spawn failed."));
}

void AG2IPipe::SpawnInteractableBoxComponent(int32 PointIndex)
{
	UBoxComponent* collisionBox = (UBoxComponent*)(AddComponentByClass(UBoxComponent::StaticClass(), false, SplineComponent->GetTransformAtSplinePoint(PointIndex, ESplineCoordinateSpace::Local), false));
	
	collisionBox->SetRelativeLocation(GetLocationBetweenPoints(PointIndex, PointIndex + 1));
	collisionBox->SetBoxExtent(FVector(CollisionBoxExtent));
	// TODO
	/*collisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap); // Pipes Custom Collision
	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &AG2IPipe::OnPipeBeginOverlap);
	collisionBox->OnComponentEndOverlap.AddDynamic(this, &AG2IPipe::OnPipeEndOverlap);*/
}

void AG2IPipe::GenerateMesh(UStaticMesh* Mesh, int32 PointIndex)
{
	USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
	SplineMesh->SetStaticMesh(Mesh);
	SplineMesh->SetMobility(EComponentMobility::Stationary);
	SplineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	SplineMesh->RegisterComponentWithWorld(GetWorld());
	SplineMesh->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Applying Mesh Transform
	FVector StartPoint, EndPoint, StartTangent, EndTangent;
	SplineComponent->GetLocationAndTangentAtSplinePoint(PointIndex, StartPoint, StartTangent, ESplineCoordinateSpace::Local);
	SplineComponent->GetLocationAndTangentAtSplinePoint(PointIndex + 1, EndPoint, EndTangent, ESplineCoordinateSpace::Local);

	SplineMesh->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, true);
	SplineMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	SplineMesh->SetForwardAxis(ForwardAxis);
	SplineMesh->SetSplineUpDir(SplineComponent->GetUpVectorAtSplinePoint(PointIndex, ESplineCoordinateSpace::Local));

	// Debug Arrows
#if WITH_EDITOR
	if (bShowDebugUpArrows)
	{
		UArrowComponent* Arrow = (UArrowComponent*)(AddComponentByClass(UArrowComponent::StaticClass(), false, SplineComponent->GetTransformAtSplinePoint(PointIndex, ESplineCoordinateSpace::Local), false));
		Arrow->SetRelativeTransform(SplineComponent->GetTransformAtSplinePoint(PointIndex, ESplineCoordinateSpace::Local));
		Arrow->SetRelativeLocationAndRotation(SplineComponent->GetLocationAtSplinePoint(PointIndex, ESplineCoordinateSpace::Local),
			SplineMesh->GetSplineUpDir().Rotation());
	}
#endif // Debug Arrows
}

void AG2IPipe::RegenerateMesh(UStaticMesh* Mesh, int32 PointIndex)
{
	TArray <USceneComponent*> SplineChildren;
	SplineComponent->GetChildrenComponents(false, SplineChildren);
	int32 s = 0;

	// Iterating through all spline components
	for (int i = 0; i < SplineChildren.Num(); i++)
	{
		// If component is USplineMeshComponent
		if (USplineMeshComponent* mesh = Cast<USplineMeshComponent>(SplineChildren[i]))
		{
			// Increment mesh components counter & check if it's the one we want to replace
			s++;
			if (s == PointIndex)
			{
				mesh->DestroyComponent();
				if (Mesh) GenerateMesh(Mesh, PointIndex);
				break;
			}
		}
	}
}

FVector AG2IPipe::GetLocationBetweenPoints(int32 Point1, int32 Point2)
{
	float pointsMiddle = (SplineComponent->GetDistanceAlongSplineAtSplinePoint(Point1) + SplineComponent->GetDistanceAlongSplineAtSplinePoint(Point2)) / 2.f;
	return SplineComponent->GetLocationAtDistanceAlongSpline(pointsMiddle, ESplineCoordinateSpace::Local);
}

void AG2IPipe::AddActorToSendAirTo(AActor* actor)
{
	ActorsToSendAirTo.Add(actor);
}

void AG2IPipe::RemoveActorFromSendAirTo(AActor* actor)
{
	ActorsToSendAirTo.Remove(actor);
}

void AG2IPipe::SendAir()
{
	for (int i = 0; i < ActorsToSendAirTo.Num(); i++)
	{
		// In Execute_RecieveAir we also call SendAir() (but not to the previous actor OR just log if loop detected)
		IG2IAirReciever::Execute_RecieveAir(ActorsToSendAirTo[i], this, GetAir());
	}
}

