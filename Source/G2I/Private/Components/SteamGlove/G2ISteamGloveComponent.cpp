#include "G2ISteamGloveComponent.h"
#include "G2I.h"
#include "G2IAimingComponent.h"
#include "G2ISteamShotComponent.h"
#include "G2IReloadingComponent.h"
#include "GameFramework/Character.h"

UG2ISteamGloveComponent::UG2ISteamGloveComponent()
{
	ReloadingComp = CreateDefaultSubobject<UG2IReloadingComponent>(FName("ReloadingComp"));
	SteamGloveAimingComp = CreateDefaultSubobject<UG2IAimingComponent>(FName("SteamGloveAimingComp"));
	SteamShotComp = CreateDefaultSubobject<UG2ISteamShotComponent>(FName("SteamShotComp"));
}

void UG2ISteamGloveComponent::OnRegister()
{
	Super::OnRegister();

	AttachGloveComponents();
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
