#include "Components/G2IVerticalMovementComponent.h"
#include "G2I.h"
#include "Components/ActorComponent.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"

float UG2IVerticalMovementComponent::FindTargetLocation()
{
	if (!ensure(Owner)) {
		UE_LOG(LogG2I, Warning, TEXT("Can't get the owner of the %s"), *this->GetName());
		return 0.0f;
	}
	USceneComponent* Root = Owner->GetRootComponent();
	if (!ensure(Root)) {
		UE_LOG(LogG2I, Warning, TEXT("Can't get the Root component of the %s"), *Owner->GetName());
		return 0.0f;
	}

	if (!ensure(TargetMeshComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("Can't get the Mesh of the %s"), *Owner->GetName());
		return 0.0f;
	}

	if (!ensure(World)) {
		UE_LOG(LogG2I, Warning, TEXT("Can't get the World"));
		return 0.0f;
	}

	FVector Origin = TargetMeshComponent->Bounds.Origin;
	FVector BoxExtent = TargetMeshComponent->Bounds.BoxExtent;
	
	TArray<FVector> StartPoints;
	StartPoints.Add(Origin + FVector(BoxExtent.X, BoxExtent.Y, -BoxExtent.Z));
	StartPoints.Add(Origin + FVector(BoxExtent.X, -BoxExtent.Y, -BoxExtent.Z));
	StartPoints.Add(Origin + FVector(-BoxExtent.X, BoxExtent.Y, -BoxExtent.Z));
	StartPoints.Add(Origin + FVector(-BoxExtent.X, -BoxExtent.Y, -BoxExtent.Z));

	ECollisionChannel TraceChannel = ECC_WorldStatic;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	FColor TraceColor = FColor::Red;
	FColor HitColor = FColor::Green;

	float Result = MovingDownDistance;
	for (const FVector& StartPoint : StartPoints) {
		FVector EndPoint = StartPoint - FVector(0, 0, MovingDownDistance);
		
		#if WITH_EDITOR
		DrawDebugLine(World, StartPoint, EndPoint, TraceColor, false, 2.0f, 0, 1.0f);
		#endif

		FHitResult Hit;
		if (World->LineTraceSingleByChannel(Hit, StartPoint, EndPoint, TraceChannel, Params)) {
			float DistToHit = StartPoint.Z - Hit.Location.Z;

			#if WITH_EDITOR
			DrawDebugPoint(World, Hit.Location, 15.0f, HitColor, false, 2.0f);
			DrawDebugLine(World, Hit.Location, Hit.Location + FVector(0, 0, 50), HitColor, false, 2.0f, 0, 2.0f);
			#endif

			if (DistToHit < Result)
			{
				Result = DistToHit;
			}
		}
	}

	return Result;
}


void UG2IVerticalMovementComponent::MoveDown()
{
	if (bIsMoving)
	{
		UE_LOG(LogG2I, Warning, TEXT("Already moving"));
		return;
	}

	if (!ensure(Owner)) {
		UE_LOG(LogG2I, Warning, TEXT("Can't get the owner of the %s"), *this->GetName());
		return;
	}
	
	if (!ensure(World)) {
		UE_LOG(LogG2I, Warning, TEXT("Can't get the World"));
		return;
	}

	float DeltaZ = FindTargetLocation();

	StartLocation = Owner->GetActorLocation();
	TargetLocation = StartLocation - FVector(0.0f, 0.0f, DeltaZ);

	bIsMoving = true;
	bIsMovingUp = false;
	World->GetTimerManager().SetTimer(
		MovementTimer,
		this,
		&UG2IVerticalMovementComponent::UpdateMovement,
		MovingVerticallyTimerInterval,
		true
	);
}

void UG2IVerticalMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	if (!ensure(World)) {
		UE_LOG(LogG2I, Warning, TEXT("Can't get the World"));
	}

	Owner = GetOwner();
	if (!ensure(Owner)) {
		UE_LOG(LogG2I, Warning, TEXT("Can't get the owner of the %s"), *this->GetName());
		return;
	}

	TargetMeshComponent = Owner->FindComponentByClass<USkeletalMeshComponent>();

	if (!ensure(TargetMeshComponent))
	{
		TargetMeshComponent = Owner->FindComponentByClass<UStaticMeshComponent>();
	}

	if (!ensure(TargetMeshComponent))
	{
		UE_LOG(LogG2I, Warning, TEXT("No mesh component found on %s"), *Owner->GetName());
	}
}


void UG2IVerticalMovementComponent::UpdateMovement()
{
	if (!bIsMoving) {
		return;
	}

	if (!ensure(World) || !ensure(Owner))
	{
		StopMovement();
		return;
	}

	float Step = MovingVerticallySpeed * MovingVerticallyTimerInterval;
	if (bIsMovingUp)
	{
		Owner->AddActorWorldOffset(FVector(0.0f, 0.0f, Step));
		if (Owner->GetActorLocation().Z >= TargetLocation.Z)
		{
			Owner->SetActorLocation(TargetLocation);
			StopMovement();

			bIsMoving = false;
			bIsMovingUp = false;
		}
	}
	else {
		Owner->AddActorWorldOffset(FVector(0.0f, 0.0f, -Step));
		if (Owner->GetActorLocation().Z <= TargetLocation.Z)
		{
			Owner->SetActorLocation(TargetLocation);
			StopMovement();
			OnMoveDownFinished.Broadcast();
			World->GetTimerManager().SetTimer(
				MovementTimer,
				this,
				&UG2IVerticalMovementComponent::MoveUp,
				ReturnDelay,
				false
			);
		}
	}
}

void UG2IVerticalMovementComponent::StopMovement()
{
	if (ensure(World))
	{
		World->GetTimerManager().ClearTimer(MovementTimer);
	}
}

void UG2IVerticalMovementComponent::MoveUp()
{
	if (!ensure(Owner)) {
		UE_LOG(LogG2I, Warning, TEXT("Can't get the owner of the %s"), *this->GetName());
		return;
	}

	if (!ensure(World)) {
		UE_LOG(LogG2I, Warning, TEXT("Can't get the World"));
		return;
	}

	if (StartLocation.IsNearlyZero() && TargetLocation.IsNearlyZero())
	{
		UE_LOG(LogG2I, Warning, TEXT("Can't call MoveUp before MoveDown in the %s"), *this->GetName());
		return;
	}
	else
	{
		TargetLocation = StartLocation;
		StartLocation = Owner->GetActorLocation();
	}

	#if WITH_EDITOR
	DrawDebugLine(World, StartLocation, TargetLocation, FColor::Red, false, 2.0f);
	#endif

	bIsMovingUp = true;
	World->GetTimerManager().SetTimer(
		MovementTimer,
		this,
		&UG2IVerticalMovementComponent::UpdateMovement,
		MovingVerticallyTimerInterval,
		true
	);
}
