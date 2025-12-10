#include "Components/G2ICharacterCollisionComponent.h"

#include "G2I.h"
#include "Components/CapsuleComponent.h"
#include "Components/G2ICharacterMovementComponent.h"
#include "GameFramework/Character.h"

void UG2ICharacterCollisionComponent::BeginPlay()
{
	Super::BeginPlay();

	BindingToDelegates();
}

void UG2ICharacterCollisionComponent::OnRegister()
{
	Super::OnRegister();

	PreInitializeDefaults();
}

UPrimitiveComponent* UG2ICharacterCollisionComponent::GetCollisionComponent()
{
	return CollisionComponent;
}

ECollisionChannel UG2ICharacterCollisionComponent::GetPassingThroughCollisionChannel()
{
	return PassingThroughCharacterCollision;
}

void UG2ICharacterCollisionComponent::OnCapsuleBeginOverlap(UPrimitiveComponent *OverlappedComponent,
	AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	OnCharacterCollisionBeginOverlap.Broadcast(OtherActor, OtherComp);
}

void UG2ICharacterCollisionComponent::OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OnCharacterCollisionEndOverlap.Broadcast(OtherActor, OtherComp);
}

void UG2ICharacterCollisionComponent::SetupPassingThroughObjectCollision()
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}
	
	UActorComponent *Component = Owner->GetComponentByClass(UG2ICharacterMovementComponent::StaticClass());
	if (!ensure(Component))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner %s hasn't movement component in %s"), *Owner->GetName(), *GetName());
		return;
	}
	
	const UG2ICharacterMovementComponent *MovementComponent = Cast<UG2ICharacterMovementComponent>(Component);
	if (!ensure(MovementComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("This movement component %s isn't target custom in %s"),
			*Component->GetName(), *GetName());
		return;
	}
	
	if (MovementComponent->CanPassThroughObject())
	{
		if (!ensure(CollisionComponent))
		{
			UE_LOG(LogG2I, Error, TEXT("Owner hasn't collision component in %s"), *GetName());
			return;
		}
		
		CollisionComponent->SetCollisionObjectType(PassingThroughCharacterCollision);
	}
}

void UG2ICharacterCollisionComponent::BindingToDelegates()
{
	if (!ensure(CollisionComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner hasn't collision component in %s"), *GetName());
		return;
	}
	
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnCapsuleBeginOverlap);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnCapsuleEndOverlap);
}

void UG2ICharacterCollisionComponent::PreInitializeDefaults()
{
	AActor *OwnerActor = GetOwner();
	if (!ensure(OwnerActor))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
		return;
	}
	
	Owner = Cast<ACharacter>(OwnerActor);
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}
	
	UCapsuleComponent *CapsuleComp = Owner->GetCapsuleComponent();
	if (!ensure(CapsuleComp))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner hasn't capsule component in %s"), *GetName());
		return;
	}
	
	CapsuleComp->InitCapsuleSize(42.f, 96.0f);
	CollisionComponent = CapsuleComp;
	if (!ensure(CollisionComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner hasn't collision component in %s"), *GetName());
		return;
	}

	SetupPassingThroughObjectCollision();
}
