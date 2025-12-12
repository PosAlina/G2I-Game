
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
	// Clearing arrays

	//  *** Remove when custom spline metadata gets fixed ***
	if (PointParams.Num() < SplineComponent->GetNumberOfSplinePoints())
	{
		int diff = SplineComponent->GetNumberOfSplinePoints() - PointParams.Num();
		PointParams.Reserve(SplineComponent->GetNumberOfSplinePoints());
		for (int i = 0; i < diff; i++)
			PointParams.AddDefaulted();
	}
	//  *** End of the custom metadata crutch ***

	// Resize and empty array of spline meshes
	SplineMeshes.Reset(SplineComponent->GetNumberOfSplinePoints());

	// Destroy & empty valves & holes actors
	for (auto& Valve : ValvesMap)
		Valve.Key->Destroy();
	ValvesMap.Empty();
	
	for (auto& Hole : HolesSet)
		Hole->Destroy();
	HolesSet.Empty();

	// Empty array of component boxes for sending/resieving air
	sendingBoxComponents.Empty();

	if (!DefaultMesh)
	{
		UE_LOG(LogG2I, Error, TEXT("Default Mesh isn't set in %s"), *GetActorNameOrLabel());
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
			SplineMeshes.Add(nullptr);
			continue;
		}

		// *	Holes / Valves Checks
		if (GetHasTechnicalHoleAtSplinePoint(PointIndex))
		{
			if (BrokenMesh)
			{
				bCanAirPassThrough = false;
				SpawnTechnicalHole(PointIndex);
				Mesh = BrokenMesh;
			}
			else
				UE_LOG(LogG2I, Error, TEXT("Broken Mesh isn't set in %s"), *GetActorNameOrLabel());
		}
		else
			Mesh = DefaultMesh;
		if (GetHasValveAtSplinePoint(PointIndex))
			SpawnValve(PointIndex);
		
		// Pipes Connections
		if (GetSendToOtherPipeAtSplinePoint(PointIndex))
		{
			UG2IPipesBoxComponent* box = SpawnPipesBoxComponent(PointIndex, false);
			sendingBoxComponents.Add(box);
		}
		if (GetReceiveFromOtherPipeAtSplinePoint(PointIndex))
		{
			UG2IPipesBoxComponent* box = SpawnPipesBoxComponent(PointIndex, true);
			sendingBoxComponents.Add(box);
		}

		// Generating Mesh
		if (PointIndex < (SplineComponent->GetNumberOfSplinePoints() - 1))
			GenerateMesh(Mesh, PointIndex);
	}
}

void AG2IPipe::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->OnWorldBeginPlay.AddUObject(this, &AG2IPipe::ForceOverlaps);
}

void AG2IPipe::ForceOverlaps()
{
	// Force call overlap events
	for (UG2IPipesBoxComponent* box : sendingBoxComponents)
	{
		TArray<UPrimitiveComponent*> OverlappingComponents;
		box->GetOverlappingComponents(OverlappingComponents);

		for (UPrimitiveComponent* otherComp : OverlappingComponents)
			OnPipeBeginOverlap(box, nullptr, otherComp, 0, false, FHitResult());
	}

	SendAir();
}

void AG2IPipe::RecieveAir_Implementation(AActor* sender, bool bAirPassed)
{
	if (ActorsToSendAirTo.Contains(sender))
	{
		UE_LOG(LogG2I, Warning, TEXT("Loop detected in %s!"), *GetActorNameOrLabel());
		return;
	}

	// Set new value in ResieveAir Map
	ReceiveAirMap.Add(sender, bAirPassed);

	// If at least one sender has air - then this pipe also has air
	// so we look for TRUE value in our map
	for (auto& resievedPair : ReceiveAirMap)
	{
		if (!resievedPair.Value)
			continue;
		
		ChangeAirPassed(true);
		return;
	}
	
	ChangeAirPassed(false);
}

void AG2IPipe::OnPipeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogG2I, Log, TEXT("Overlap in %s Actor"), *GetActorNameOrLabel());

	UG2IPipesBoxComponent* box = Cast<UG2IPipesBoxComponent>(OverlappedComp);
	UG2IPipesBoxComponent* otherBox = Cast<UG2IPipesBoxComponent>(OtherComp);

	if (box && otherBox)
	{
		if (!otherBox->Owner)
		{
			UE_LOG(LogG2I, Warning, TEXT("Owner in Box Component %s is NULL."), *otherBox->GetName());
			return;
		}

		if (!box->bRecieves && otherBox->bRecieves)
			ActorsToSendAirTo.AddUnique(otherBox->Owner);
		else if (box->bRecieves && !otherBox->bRecieves)
			ReceiveAirMap.Add(otherBox->Owner, false);
	}
}

void AG2IPipe::OnPipeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogG2I, Log, TEXT("End overlap in %s Actor"), *GetActorNameOrLabel());

	if (UG2IPipesBoxComponent* box = Cast<UG2IPipesBoxComponent>(OtherComp))
	{
		if (box->bRecieves)
			ActorsToSendAirTo.Remove(box->Owner);
		else
			ReceiveAirMap.Remove(box->Owner);
	}
}

TObjectPtr<UG2IPipesSplineMetadata> AG2IPipe::GetSplineMetadata() const
{
	return SplineMetadata;
}

void AG2IPipe::ChangeAirPassed(bool newAirPassed)
{
	if (bHasAirPassed != newAirPassed)
	{
		bHasAirPassed = newAirPassed;
		SendAir();
	}
}

void AG2IPipe::ChangeCanAirPass(bool newCanAirPass)
{
	if (bCanAirPassThrough != newCanAirPass)
	{
		bCanAirPassThrough = newCanAirPass;
		SendAir();
	}
}

void AG2IPipe::CheckIfAirCanPass()
{
	// Check for pipes with holes
	for (auto& PointsValue : PointParams)
	{
		if (!PointsValue.bHasTechnicalHole)
			continue;

		ChangeCanAirPass(false);
		return;
	}

	// If at least one valve is inactive - air is not passing through
	for (auto& ValvePair : ValvesMap)
	{
		if (ValvePair.Value)
			continue;

		ChangeCanAirPass(false);
		return;
	}

	ChangeCanAirPass(true);
}

bool AG2IPipe::GetAir() const
{
	UE_LOG(LogG2I, Log, TEXT("bAir changed to %d in %s Actor"), bHasAirPassed && bCanAirPassThrough, *GetActorNameOrLabel());
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

FRotator AG2IPipe::GetValveRotationAtSplinePoint(int32 PointIndex)
{
	if (ensure(PointParams.IsValidIndex(PointIndex)))
	{
		return PointParams[PointIndex].ValveRotation;
	}

	return FRotator(0.0);
}

bool AG2IPipe::GetValveActivatedAtSplinePoint(int32 PointIndex)
{
	if (ensure(PointParams.IsValidIndex(PointIndex)))
	{
		return PointParams[PointIndex].bValveActivated;
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

void AG2IPipe::SetHasTechnicalHoleAtSplinePoint(int32 PointIndex, bool newHasTechnicalHole)
{
	if (ensure(PointParams.IsValidIndex(PointIndex)))
	{
		PointParams[PointIndex].bHasTechnicalHole = newHasTechnicalHole;
	}
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

bool AG2IPipe::GetReceiveFromOtherPipeAtSplinePoint(int32 PointIndex)
{
	if (ensure(PointParams.IsValidIndex(PointIndex)))
	{
		return PointParams[PointIndex].bReceiveFromOtherPipe;
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

void AG2IPipe::SpawnTechnicalHole(int32 PointIndex)
{
	if (!HoleClass)
	{
		UE_LOG(LogG2I, Error, TEXT("Hole Class isn't set in %s"), *GetActorNameOrLabel());
		return;
	}

	// Set Spawn Settings
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Spawn Hole Actor
	AG2ITechnicalHole* Hole = GetWorld()->SpawnActor<AG2ITechnicalHole>(HoleClass, GetLocationBetweenPoints(PointIndex, PointIndex + 1, ESplineCoordinateSpace::World), FRotator(0.f), SpawnParams);
	Hole->PointIndex = PointIndex;
	HolesSet.Add(Hole);
}

void AG2IPipe::SpawnValve(int32 PointIndex)
{
	if (!ValveClass)
	{
		UE_LOG(LogG2I, Warning, TEXT("Valve Class isn't set in %s"), *GetActorNameOrLabel());
		return;
	}

	// Set Spawn Settings
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Spawn Valve Actor
	AG2IValve* Valve = GetWorld()->SpawnActor<AG2IValve>(ValveClass, GetLocationBetweenPoints(PointIndex, PointIndex + 1, ESplineCoordinateSpace::World), GetValveRotationAtSplinePoint(PointIndex), SpawnParams);
	Valve->OwnerActor = this;
	Valve->bActivated = GetValveActivatedAtSplinePoint(PointIndex);
	ValvesMap.Add(Valve, Valve->bActivated);
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
	SplineMesh->SetCollisionProfileName(TEXT("BlockAll"));

	SplineMesh->SetForwardAxis(ForwardAxis);
	SplineMesh->SetSplineUpDir(SplineComponent->GetUpVectorAtSplinePoint(PointIndex, ESplineCoordinateSpace::Local));

	SplineMeshes.Add(SplineMesh);

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
	TObjectPtr<USplineMeshComponent> SplineMesh = SplineMeshes[PointIndex];

	// If Spline has SplineMeshComponent at needed point
	if (SplineMesh)
	{
		if (Mesh) // If Mesh is valid
			SplineMesh->SetStaticMesh(Mesh); // Switch Static Mesh
		else
		{
			SplineMesh->DestroyComponent();  // Destroy SplineMeshComponent
			SplineMeshes[PointIndex] = nullptr;
		}
	}
	else
		GenerateMesh(Mesh, PointIndex); // Generate Spline Mesh Component and set SM
}

FVector AG2IPipe::GetLocationBetweenPoints(int32 Point1, int32 Point2, ESplineCoordinateSpace::Type CoordSpace)
{
	float pointsMiddle = (SplineComponent->GetDistanceAlongSplineAtSplinePoint(Point1) + SplineComponent->GetDistanceAlongSplineAtSplinePoint(Point2)) / 2.f;
	return SplineComponent->GetLocationAtDistanceAlongSpline(pointsMiddle, CoordSpace);
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
		// In Execute_RecieveAir we also call SendAir()
		IG2IAirReciever::Execute_RecieveAir(ActorsToSendAirTo[i], this, GetAir());
	}
}

void AG2IPipe::OnValveActivationChanged(AG2IValve* Valve, bool newActivated)
{
	ValvesMap.Add(Valve, newActivated);

	CheckIfAirCanPass();
}

