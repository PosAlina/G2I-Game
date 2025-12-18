
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
	if (!SplineComponent)
	{
		UE_LOG(LogG2I, Error, TEXT("SplineComponent is null in %s"), *GetActorNameOrLabel());
		return;
	}

	// Setting up arrays

	//  *** Remove when custom spline metadata gets fixed ***
	if (PointParams.Num() < SplineComponent->GetNumberOfSplinePoints())
	{
		int Diff = SplineComponent->GetNumberOfSplinePoints() - PointParams.Num();
		PointParams.Reserve(SplineComponent->GetNumberOfSplinePoints());
		for (int i = 0; i < Diff; i++)
			PointParams.AddDefaulted();
	}
	else if (PointParams.Num() > SplineComponent->GetNumberOfSplinePoints())
	{
		int Diff = PointParams.Num() - SplineComponent->GetNumberOfSplinePoints();
		PointParams.RemoveAt<int>(SplineComponent->GetNumberOfSplinePoints(), Diff, EAllowShrinking::Yes);
	}
	//  *** End of the custom metadata crutch ***

	// Resize and empty array of spline meshes
	SplineMeshes.Reset(SplineComponent->GetNumberOfSplinePoints());

	// Empty array of component boxes for sending/resieving air
	SendingBoxComponents.Empty();

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

#if WITH_EDITOR
		// *	Valves Check & Spawn Editor arrow for easy-to-see edits
		if (GetHasValveAtSplinePoint(PointIndex))
		{
			UArrowComponent* Arrow = (UArrowComponent*)(AddComponentByClass(UArrowComponent::StaticClass(), false, SplineComponent->GetTransformAtSplinePoint(PointIndex, ESplineCoordinateSpace::Local), false));
			Arrow->SetRelativeLocationAndRotation(GetLocationBetweenPoints(PointIndex, PointIndex + 1),
				GetValveRotationAtSplinePoint(PointIndex).Add(90., 0., 0.));
			Arrow->SetRelativeScale3D(FVector(0.3));
			Arrow->ArrowColor = FColor::Cyan;
		}
#endif

		// *    Holes Check
		if (GetHasTechnicalHoleAtSplinePoint(PointIndex))
		{
			if (BrokenMesh)
			{
				bCanAirPassThrough = false;
				Mesh = BrokenMesh;
			}
			else
				UE_LOG(LogG2I, Error, TEXT("Broken Mesh isn't set in %s"), *GetActorNameOrLabel());
		}
		else
			Mesh = DefaultMesh;
		
		// *    Pipes Connections
		if (GetSendToOtherPipeAtSplinePoint(PointIndex))
		{
			UG2IPipesBoxComponent* Box = SpawnPipesBoxComponent(PointIndex, false);
			SendingBoxComponents.Add(Box);
		}
		if (GetReceiveFromOtherPipeAtSplinePoint(PointIndex))
		{
			UG2IPipesBoxComponent* Box = SpawnPipesBoxComponent(PointIndex, true);
			SendingBoxComponents.Add(Box);
		}

		// Generating Mesh
		if (PointIndex < (SplineComponent->GetNumberOfSplinePoints() - 1))
			GenerateMesh(Mesh, PointIndex);
	}
}

void AG2IPipe::BeginPlay()
{
	Super::BeginPlay();

	// Since Holes and Valves are actors - we spawn them during Runtime
	SpawnValves();
	SpawnTechnicalHoles();
	
	// Checking world on null
	if (!GetWorld())
	{
		UE_LOG(LogG2I, Error, TEXT("No World during %s's BeginPlay"), *GetActorNameOrLabel());
		return;
	}

	// Forcing Overlap Events for pipes to connect to each other
	if (GetWorld()->GetBegunPlay())
	{
		UE_LOG(LogG2I, Verbose, TEXT("World's BeginPlay already ended during %s's BeginPlay, starting ForceOverlaps"), *GetActorNameOrLabel());
		ForceOverlaps();
	}
	else
	{
		UE_LOG(LogG2I, Verbose, TEXT("World exists but still hasn't played BeginPlay during %s's BeginPlay, subscribing to OnWorldBeginPlay"), *GetActorNameOrLabel());
		GetWorld()->OnWorldBeginPlay.AddUObject(this, &AG2IPipe::ForceOverlaps);
	}
}

void AG2IPipe::ForceOverlaps()
{
	UE_LOG(LogG2I, Log, TEXT("ForceOverlaps called in %s"), *GetActorNameOrLabel());

	// Force call overlap events
	for (UG2IPipesBoxComponent* Box : SendingBoxComponents)
	{
		TArray<UPrimitiveComponent*> OverlappingComponents;
		Box->GetOverlappingComponents(OverlappingComponents);

		for (UPrimitiveComponent* OtherComp : OverlappingComponents)
			OnPipeBeginOverlap(Box, nullptr, OtherComp, 0, false, FHitResult());
	}

	SendAir();
}

void AG2IPipe::SpawnValves()
{
	if (ensure(SplineComponent))
		for (int i = 0; i < SplineComponent->GetNumberOfSplinePoints() - 1; i++)
		{
			if (GetHasValveAtSplinePoint(i))
				SpawnValve(i);
		}
}

void AG2IPipe::SpawnTechnicalHoles()
{
	if (ensure(SplineComponent))
		for (int i = 0; i < SplineComponent->GetNumberOfSplinePoints() - 1; i++)
		{
			if (GetHasTechnicalHoleAtSplinePoint(i))
				SpawnTechnicalHole(i);
		}
}

void AG2IPipe::RecieveAir_Implementation(AActor* Sender, bool bAirPassed)
{
	if (ActorsToSendAirTo.Contains(Sender))
	{
		UE_LOG(LogG2I, Warning, TEXT("Loop detected in %s!"), *GetActorNameOrLabel());
		return;
	}

	// Set new value in ResieveAir Map
	ReceiveAirMap.Add(Sender, bAirPassed);

	// If at least one sender has air - then this pipe also has air
	// so we look for TRUE value in our map
	for (auto& ResievedPair : ReceiveAirMap)
	{
		if (!ResievedPair.Value)
			continue;
		
		ChangeAirPassed(true);
		return;
	}
	
	ChangeAirPassed(false);
}

void AG2IPipe::OnPipeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogG2I, Log, TEXT("Overlap in %s Actor"), *GetActorNameOrLabel());

	UG2IPipesBoxComponent* Box = Cast<UG2IPipesBoxComponent>(OverlappedComp);
	UG2IPipesBoxComponent* OtherBox = Cast<UG2IPipesBoxComponent>(OtherComp);

	if (ensure(Box && OtherBox))
	{
		if (!OtherBox->Owner)
		{
			UE_LOG(LogG2I, Warning, TEXT("Owner in Box Component %s is NULL."), *OtherBox->GetName());
			return;
		}

		if (!Box->bRecieves && OtherBox->bRecieves)
			ActorsToSendAirTo.AddUnique(OtherBox->Owner);
		else if (Box->bRecieves && !OtherBox->bRecieves)
			ReceiveAirMap.Add(OtherBox->Owner, false);
	}
}

void AG2IPipe::OnPipeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogG2I, Verbose, TEXT("End overlap in %s Actor"), *GetActorNameOrLabel());

	if (UG2IPipesBoxComponent* Box = Cast<UG2IPipesBoxComponent>(OtherComp))
	{
		if (Box->bRecieves)
			ActorsToSendAirTo.Remove(Box->Owner);
		else
			ReceiveAirMap.Remove(Box->Owner);
	}
}

TObjectPtr<UG2IPipesSplineMetadata> AG2IPipe::GetSplineMetadata() const
{
	return SplineMetadata;
}

void AG2IPipe::ChangeAirPassed(bool bNewAirPassed)
{
	if (bHasAirPassed != bNewAirPassed)
	{
		bHasAirPassed = bNewAirPassed;
		SendAir();
	}
}

void AG2IPipe::ChangeCanAirPass(bool bNewCanAirPass)
{
	if (bCanAirPassThrough != bNewCanAirPass)
	{
		bCanAirPassThrough = bNewCanAirPass;
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
	UE_LOG(LogG2I, Verbose, TEXT("bAir changed to %d in %s Actor"), bHasAirPassed && bCanAirPassThrough, *GetActorNameOrLabel());
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
	UG2IPipesBoxComponent* CollisionBox = (UG2IPipesBoxComponent*)(AddComponentByClass(UG2IPipesBoxComponent::StaticClass(), false, SplineComponent->GetTransformAtSplinePoint(PointIndex, ESplineCoordinateSpace::Local), false));
	
	if (ensure(CollisionBox))
	{
		CollisionBox->SetBoxExtent(FVector(CollisionBoxExtent));
		CollisionBox->bRecieves = bRecieves;
		CollisionBox->Owner = this;
		CollisionBox->PointIndex = PointIndex;
		CollisionBox->SetGenerateOverlapEvents(true);
		CollisionBox->SetCollisionObjectType(ECC_GameTraceChannel3);					 // Pipes Custom Collision
		CollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap); // Pipes Custom Collision
		CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AG2IPipe::OnPipeBeginOverlap);
		CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AG2IPipe::OnPipeEndOverlap);
	}

	return CollisionBox;
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
	if (GetWorld())
	{
		AG2ITechnicalHole* Hole = GetWorld()->SpawnActor<AG2ITechnicalHole>(HoleClass, GetLocationBetweenPoints(PointIndex, PointIndex + 1, ESplineCoordinateSpace::World), FRotator(0.f), SpawnParams);

		if (ensure(Hole))
		{
			Hole->PointIndex = PointIndex;
			HolesSet.Add(Hole);
		}
	}
}

void AG2IPipe::SpawnValve(int32 PointIndex)
{
	if (!ValveClass)
	{
		UE_LOG(LogG2I, Error, TEXT("Valve Class isn't set in %s"), *GetActorNameOrLabel());
		return;
	}

	// Set Spawn Settings
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Spawn Valve Actor
	if (GetWorld())
	{
		AG2IValve* Valve = GetWorld()->SpawnActor<AG2IValve>(ValveClass, GetLocationBetweenPoints(PointIndex, PointIndex + 1, ESplineCoordinateSpace::World), GetValveRotationAtSplinePoint(PointIndex), SpawnParams);

		if (ensure(Valve))
		{
			Valve->OwnerActor = this;
			Valve->bActivated = GetValveActivatedAtSplinePoint(PointIndex);
			ValvesMap.Add(Valve, Valve->bActivated);
		}
	}
}

void AG2IPipe::SpawnInteractableBoxComponent(int32 PointIndex)
{
	UBoxComponent* CollisionBox = (UBoxComponent*)(AddComponentByClass(UBoxComponent::StaticClass(), false, SplineComponent->GetTransformAtSplinePoint(PointIndex, ESplineCoordinateSpace::Local), false));
	
	if (ensure(CollisionBox))
	{
		CollisionBox->SetRelativeLocation(GetLocationBetweenPoints(PointIndex, PointIndex + 1));
		CollisionBox->SetBoxExtent(FVector(CollisionBoxExtent));
		// TODO
		/*collisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap); // Pipes Custom Collision
		collisionBox->OnComponentBeginOverlap.AddDynamic(this, &AG2IPipe::OnPipeBeginOverlap);
		collisionBox->OnComponentEndOverlap.AddDynamic(this, &AG2IPipe::OnPipeEndOverlap);*/
	}
}

void AG2IPipe::GenerateMesh(UStaticMesh* Mesh, int32 PointIndex)
{
	USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());

	if (ensure(SplineMesh))
	{
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
			if (Arrow)
				Arrow->SetRelativeLocationAndRotation(SplineComponent->GetLocationAtSplinePoint(PointIndex, ESplineCoordinateSpace::Local),
					SplineMesh->GetSplineUpDir().Rotation());
		}
#endif // Debug Arrows
	}
}

void AG2IPipe::RegenerateMesh(UStaticMesh* Mesh, int32 PointIndex)
{
	if (ensure(SplineMeshes.IsValidIndex(PointIndex)))
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
}

FVector AG2IPipe::GetLocationBetweenPoints(int32 Point1, int32 Point2, ESplineCoordinateSpace::Type CoordSpace)
{
	if (ensure(SplineComponent))
	{
		float PointsMiddle = (SplineComponent->GetDistanceAlongSplineAtSplinePoint(Point1) + SplineComponent->GetDistanceAlongSplineAtSplinePoint(Point2)) / 2.f;
		return SplineComponent->GetLocationAtDistanceAlongSpline(PointsMiddle, CoordSpace);
	}
	else
		return FVector();
}

void AG2IPipe::AddActorToSendAirTo(AActor* Actor)
{
	ActorsToSendAirTo.Add(Actor);
}

void AG2IPipe::RemoveActorFromSendAirTo(AActor* Actor)
{
	ActorsToSendAirTo.Remove(Actor);
}

void AG2IPipe::SendAir()
{
	for (int i = 0; i < ActorsToSendAirTo.Num(); i++)
	{
		// In Execute_RecieveAir we also call SendAir()
		IG2IAirRecieverInterface::Execute_RecieveAir(ActorsToSendAirTo[i], this, GetAir());
	}
}

void AG2IPipe::OnValveActivationChanged(AG2IValve* Valve, bool bNewActivated)
{
	ValvesMap.Add(Valve, bNewActivated);

	CheckIfAirCanPass();
}

