#include "Components/G2IGrabberComponent.h"
#include "G2I.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/G2IVerticalMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"

UG2IGrabberComponent::UG2IGrabberComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	VerticalMovementComp = CreateDefaultSubobject<UG2IVerticalMovementComponent>(TEXT("VerticalMovementComp"));
	PhysicsHandleComp = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandleComp"));

	Anchor = CreateDefaultSubobject<UBoxComponent>(TEXT("AnchorPoint"));
	if (!ensure(Anchor)) {
		UE_LOG(LogG2I, Error, TEXT("Can't create BoxComponent in the %s"), *this->GetName());
		return;
	}
	Anchor->SetupAttachment(this);

	Anchor->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Anchor->SetCollisionResponseToAllChannels(ECR_Overlap);
	Anchor->SetMobility(EComponentMobility::Movable);
}


void UG2IGrabberComponent::PlayHookAnimation(UAnimSequence* Animation)
{
	if (!ensure(SkeletalMeshComp) || !ensure(Animation)) {
		UE_LOG(LogG2I, Error, TEXT("Can't play Animation in the %s"), *this->GetName());
		return;
	}

	SkeletalMeshComp->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	SkeletalMeshComp->PlayAnimation(Animation, false);
}

void UG2IGrabberComponent::FinishGrab(UPrimitiveComponent* OtherComp, FVector Location, AActor* OtherActor)
{
	if (!ensure(PhysicsHandleComp)) {
		UE_LOG(LogG2I, Error, TEXT("PhysicsHandleComp is null for %s"), *GetName());
		return;
	}

	PhysicsHandleComp->GrabComponentAtLocation(OtherComp, NAME_None, Location);

	StartLocation = Location;
	TargetLocation = Anchor->GetComponentLocation();
	CurrentGrabTime = 0.0f;
	
	if (!ensure(World)) {
		UE_LOG(LogG2I, Error, TEXT("Can't get the World"));
		return;
	}
	World->GetTimerManager().SetTimer(
		MovementTimer,
		this,
		&UG2IGrabberComponent::UpdateSmoothMovement,
		UpdateTime,
		true
	);

	GrabbedActor = OtherActor;
}

void UG2IGrabberComponent::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	if (!ensure(World)) {
		UE_LOG(LogG2I, Error, TEXT("Can't get the World"));
	}

	Owner = GetOwner();
	if (!ensure(Owner)) {
		UE_LOG(LogG2I, Error, TEXT("Can't get the owner of the %s"), *GetName());
		return;
	}

	SkeletalMeshComp = Owner->FindComponentByClass<USkeletalMeshComponent>();
	if (!ensure(SkeletalMeshComp))
	{
		UE_LOG(LogG2I, Error, TEXT("No SkeletalMeshComponent found on %s"), *Owner->GetName());
	}

	if (!ensure(Anchor)) {
		UE_LOG(LogG2I, Error, TEXT("Anchor isn't defined for the %s"), *Owner->GetName());
		return;
	}

	Anchor->OnComponentBeginOverlap.AddDynamic(this, &UG2IGrabberComponent::OnOverlapBegin);

	if (ensure(VerticalMovementComp))
	{
		VerticalMovementComp->OnMoveDownFinished.AddDynamic(this, &UG2IGrabberComponent::OnMoveDownFinished);
	}
}



void UG2IGrabberComponent::UpdateSmoothMovement()
{
	CurrentGrabTime += UpdateTime;
	if (CurrentGrabTime >= MaxGrabTime) {
		if (!ensure(World)) {
			UE_LOG(LogG2I, Error, TEXT("Can't get the World"));
			return;
		}

		if (!ensure(PhysicsHandleComp)) {
			UE_LOG(LogG2I, Error, TEXT("PhysicsHandleComp is null for %s"), *GetName());
			return;
		}

		World->GetTimerManager().ClearTimer(MovementTimer);
		PhysicsHandleComp->SetTargetLocation(TargetLocation);

		if (!bRemovePhysicsHandleAfterGrab) {
			SetComponentTickEnabled(true);

		}
		else {
			if (ensure(PhysicsHandleComp)) {
				PhysicsHandleComp->ReleaseComponent();
			}
		}

		return;
	}

	float Alpha = CurrentGrabTime / MaxGrabTime;
	Alpha = FMath::InterpEaseInOut(0.0f, 1.0f, Alpha, 2.0f);
	FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);
	PhysicsHandleComp->SetTargetLocation(NewLocation);
}

void UG2IGrabberComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!ensure(Anchor)) {
		UE_LOG(LogG2I, Error, TEXT("Anchor isn't created for the %s"), *GetName());
		return;
	}

	if (!ensure(PhysicsHandleComp)) {
		UE_LOG(LogG2I, Error, TEXT("PhysicsHandleComp is null for %s"), *GetName());
		return;
	}

	FVector NewLocation = Anchor->GetComponentLocation();
	PhysicsHandleComp->SetTargetLocation(NewLocation);
}

void UG2IGrabberComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ensure(GrabbedActor)) {
		UE_LOG(LogG2I, Verbose, TEXT("Overlap ignored - already have grabbed actor: %s"), *GrabbedActor->GetName());
		return;
	}

	if (!ensure(OtherActor)) {
		UE_LOG(LogG2I, Warning, TEXT("Overlap with null actor"));
		return;
	}

	if (!ensure(OtherActor->ActorHasTag(FName("CanBeGrabbed")))) {
		UE_LOG(LogG2I, Verbose, TEXT("Actor %s doesn't have CanBeGrabbed tag"), *OtherActor->GetName());
		return;
	}

	if (!ensure(World)) {
		UE_LOG(LogG2I, Error, TEXT("Can't get the World in OnOverlapBegin"));
		return;
	}

	if (!ensure(OtherComp)) {
		UE_LOG(LogG2I, Error, TEXT("OtherComp is null for actor %s"), *OtherActor->GetName());
		return;
	}

	if (!ensure(PhysicsHandleComp)) {
		UE_LOG(LogG2I, Error, TEXT("PhysicsHandleComp is null in OnOverlapBegin"));
		return;
	}

	FVector Location = OtherComp->GetComponentLocation();
	FTimerHandle DelayHandle;

	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &UG2IGrabberComponent::FinishGrab, OtherComp, Location, OtherActor);

	World->GetTimerManager().SetTimer(DelayHandle, Delegate, 0.2f, false);
}

void UG2IGrabberComponent::Activate_Implementation()
{
	if (bHookIsClosed) {
		PlayHookAnimation(OpenHookAnimation);
		bHookIsClosed = false;
		if (ensure(PhysicsHandleComp)) {
			PhysicsHandleComp->ReleaseComponent();
		}
		else {
			UE_LOG(LogG2I, Error, TEXT("PhysicsHandleComp is null during release"));
		}

		GrabbedActor = nullptr;

		SetComponentTickEnabled(false);

		if (ensure(World) && MovementTimer.IsValid()) {
			World->GetTimerManager().ClearTimer(MovementTimer);
		}
	}
	else {
		if (ensure(VerticalMovementComp)) {
			VerticalMovementComp->MoveDown();
		}
		else {
			UE_LOG(LogG2I, Error, TEXT("VerticalMovementComp is null during MoveDown"));
		}
	}
}

void UG2IGrabberComponent::Deactivate_Implementation()
{
	return;
}

void UG2IGrabberComponent::OnMoveDownFinished()
{
	PlayHookAnimation(CloseHookAnimation);
	bHookIsClosed = true;
}