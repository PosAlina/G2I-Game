#include "G2ISteamGloveComponent.h"
#include "G2I.h"
#include "G2IAimingComponent.h"
#include "G2IGlovePunchComponent.h"
#include "G2ISteamShotComponent.h"
#include "GameFramework/Character.h"

#if WITH_EDITORONLY_DATA
#include "G2ISteamMovementComponent.h"
#include "G2IReloadingComponent.h"
#endif

UG2ISteamGloveComponent::UG2ISteamGloveComponent()
{
	SteamGloveAimingComp = CreateDefaultSubobject<UG2IAimingComponent>(FName("SteamGloveAimingComp"));
	SteamShotComp = CreateDefaultSubobject<UG2ISteamShotComponent>(FName("SteamShotComp"));
	GlovePunchComp = CreateDefaultSubobject<UG2IGlovePunchComponent>(FName("GlovePunchComp"));

	bWantsInitializeComponent = true;
}

void UG2ISteamGloveComponent::InitializeComponent()
{
	Super::InitializeComponent();

	AttachGloveComponents();

#if WITH_EDITOR
	SteamMovementComp = NewObject<UG2ISteamMovementComponent>(this, UG2ISteamMovementComponent::StaticClass(), FName("SteamMovementComp"));
	ReloadingComp = NewObject<UG2IReloadingComponent>(this, UG2IReloadingComponent::StaticClass(), FName("ReloadingComp"));
#endif
}

void UG2ISteamGloveComponent::AttachGloveComponents()
{
	AActor *OwnerActor = GetOwner();
	if (!ensure(OwnerActor))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
		return;
	}
	const ACharacter *Owner = Cast<ACharacter>(OwnerActor);
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("%s isn't character in %s"), *OwnerActor->GetActorNameOrLabel(),
			*GetName());
		return;
	}

	USkeletalMeshComponent *OwnerMesh = Owner->GetMesh();
	if (OwnerMesh && OwnerMesh->GetBoneIndex(AttachBoneName) != INDEX_NONE)
	{
		AttachToComponent(OwnerMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachBoneName);
	}
	else
	{
		AttachToOwnerRootComponent(this);
	}
	
	if (OwnerMesh && OwnerMesh->GetBoneIndex(AttachBoneNameShotComponent) != INDEX_NONE)
	{
		SteamShotComp->AttachToComponent(OwnerMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachBoneNameShotComponent);
	}
	else
	{
		AttachToOwnerRootComponent(SteamShotComp);
	}
}

void UG2ISteamGloveComponent::AttachToOwnerRootComponent(USceneComponent* AttachableComponent)
{
	const AActor *OwnerActor = GetOwner();
	if (!ensure(OwnerActor))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
		return;
	}
	USceneComponent *RootComponent = OwnerActor->GetRootComponent();
	if (!ensure(RootComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner %s doesn't have root component in %s"),
			*OwnerActor->GetActorNameOrLabel(), *GetName());
		return;
	}

	AttachableComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}
