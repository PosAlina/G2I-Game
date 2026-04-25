#include "G2ICharacterCarryingActorsComponent.h"
#include "G2I.h"
#include "G2IHandsMovableComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

UG2ICharacterCarryingActorsComponent::UG2ICharacterCarryingActorsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	 
	ColliderBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ColliderBox"));
	if (!ensure(ColliderBox))
	{
		UE_LOG(LogG2I, Error, TEXT("ColliderBox can't created in %s"), *GetName());
		return;
	}
	
#if WITH_EDITOR
	ColliderBox->SetHiddenInGame(false);
	ColliderBox->SetVisibility(true);
#endif
}

bool UG2ICharacterCarryingActorsComponent::PickUp(
	AActor& TargetActor, UG2IHandsMovableComponent& InHandsMovableComponent,
	bool bIsOverrideMass, float OverridenMass,
	bool bIsOverrideLocationInHands, const FVector& OverridenLocationInHands,
	bool bIsOverrideRotationInHands, const FRotator& OverridenRotationInHands)
{
	G2I::DebugLogMessage("Pick up " + TargetActor.GetActorNameOrLabel());
	
	HandsMovableComponent = &InHandsMovableComponent;
	GrabbedActor = &TargetActor;
	
	UPrimitiveComponent *ActorPrimitive = TargetActor.FindComponentByClass<UPrimitiveComponent>();
	if (!ensure(ActorPrimitive))
	{
		G2I::DebugWarningMessage(
			"Can't pick up " + TargetActor.GetActorNameOrLabel() + ". It hasn't primitive component");
		return false;
	}
	
	InHandsMovableComponent.PrepareBeforeGrabPickedUp();
	GrabComponentAtLocationWithRotation(ActorPrimitive, AttachBoneName, TargetActor.GetActorLocation(), OverridenRotationInHands);
	SetCollisionForCharacterWthGrabbedActor(*ActorPrimitive);
	
	/*InHandsMovableComponent.PrepareBeforeAttachedPickedUp()
	TargetActor.AttachToActor(GetOwner(), FAttachmentTransformRules::KeepWorldTransform, AttachBoneName);*/

	ColliderBox->SetGenerateOverlapEvents(true);
	bHandsAreBusy = true;
	return true;
}

bool UG2ICharacterCarryingActorsComponent::PutDown()
{
	HandsMovableComponent->PrepareBeforePutDown();
	ReleaseComponent();
	
	/*const FDetachmentTransformRules DetachmentTransformRules = FDetachmentTransformRules::KeepWorldTransform;
	GrabbedActor->DetachFromActor(DetachmentTransformRules);*/

	ColliderBox->SetGenerateOverlapEvents(false);
	bHandsAreBusy = false;
	return false;
}

void UG2ICharacterCarryingActorsComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Owner = Cast<ACharacter>(GetOwner());
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
		return;
	}
	OwnerCapsule = Owner->FindComponentByClass<UCapsuleComponent>();
	if (!ensure(OwnerCapsule))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner %s doesn't have capsule in %s"), *Owner->GetActorNameOrLabel(), *GetName());
		return;
	}
	
	ColliderBox->AttachToComponent(OwnerCapsule, FAttachmentTransformRules::KeepWorldTransform);
	//ColliderBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//ColliderBox->SetCollisionObjectType(ECC_GameTraceChannel8);
	//ColliderBox->SetCollisionProfileName("BlockAllDynamic");
	//ColliderBox->SetCollisionResponseToAllChannels(ECR_Block);
	//ColliderBox->SetBoxExtent(FVector(100, 100, 100));
	//ColliderBox->SetWorldLocation(OwnerCapsule->GetComponentLocation());
	//ColliderBox->SetGenerateOverlapEvents(true);*/
	
	/*ColliderBox->SetCollisionObjectType(OwnerCapsule->GetCollisionObjectType());
	ColliderBox->SetCollisionProfileName(OwnerCapsule->GetCollisionProfileName());
	ColliderBox->SetCollisionProfileName(OwnerCapsule->GetCollisionProfileName());*/
}

void UG2ICharacterCarryingActorsComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GrabbedComponent)
	{
		if (!ensure(Owner))
		{
			UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
			return;
		}
		
		FVector HoldLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 100.f;
		HoldLocation.Z += 50.f;
		SetTargetLocation(HoldLocation);
		FRotator HoldRotation = Owner->GetActorRotation();
		HoldRotation.Roll = 0;
		HoldRotation.Pitch = 0;
		SetTargetRotation(HoldRotation);
	}
}

void UG2ICharacterCarryingActorsComponent::SetCollisionForCharacterWthGrabbedActor(const UPrimitiveComponent& ActorPrimitive) const
{
	if (!ensure(OwnerCapsule))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner %s doesn't have capsule in %s"), *Owner->GetActorNameOrLabel(), *GetName());
		return;
	}
	
	const FBoxSphereBounds Bound1 = OwnerCapsule->GetLocalBounds();
	const FBoxSphereBounds Bound2 = ActorPrimitive.GetLocalBounds();

	const FVector CapsuleLocation = OwnerCapsule->GetComponentLocation();
	const FVector GrabbedLocation = ActorPrimitive.GetComponentLocation();
	
	FVector ColliderLocation;
	FVector CommonBoxExtent;

	const float MinZ = FMath::Min(CapsuleLocation.Z - Bound1.BoxExtent.Z, GrabbedLocation.Z - Bound2.BoxExtent.Z);
	const float MaxZ = FMath::Max(CapsuleLocation.Z + Bound1.BoxExtent.Z, GrabbedLocation.Z + Bound2.BoxExtent.Z);
	CommonBoxExtent.Z = MaxZ - MinZ;
	CommonBoxExtent.Z *= 0.5;
	ColliderLocation.Z = MinZ + CommonBoxExtent.Z;


	const float MinX = FMath::Min(CapsuleLocation.X - Bound1.BoxExtent.X, GrabbedLocation.X - Bound2.BoxExtent.X);
	const float MaxX = FMath::Max(CapsuleLocation.X + Bound1.BoxExtent.X, GrabbedLocation.X + Bound2.BoxExtent.X);
	CommonBoxExtent.X = MaxX - MinX;
	CommonBoxExtent.X *= 0.5;
	ColliderLocation.X = MinX + CommonBoxExtent.X;

	const float MinY = FMath::Min(CapsuleLocation.Y - Bound1.BoxExtent.Y, GrabbedLocation.Y - Bound2.BoxExtent.Y);
	const float MaxY = FMath::Max(CapsuleLocation.Y + Bound1.BoxExtent.Y, GrabbedLocation.Y + Bound2.BoxExtent.Y);
	CommonBoxExtent.Y = MaxY - MinY;
	CommonBoxExtent.Y *= 0.5;
	ColliderLocation.Y = MinY + CommonBoxExtent.Y;

	ColliderBox->SetWorldLocation(ColliderLocation);
	ColliderBox->SetBoxExtent(CommonBoxExtent);
}
