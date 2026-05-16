#include "AnimNotify/G2IFootstepNotify.h"
#include "G2IFootstepsComponent.h"
#include "G2I.h"

void UG2IFootstepNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: USkeletalMeshComponent is null."), *GetName());
		return;
	}

	const AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: SkeletalMeshComponent's Owner is null."), *GetName());
		return;
	}

	UWorld* World = Owner->GetWorld();
	if (!World)
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: World is null."), *GetName());
		return;
	}

	UG2IFootstepsComponent* FootstepsComp = Owner->GetComponentByClass<UG2IFootstepsComponent>();
	if (!FootstepsComp)
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Couldn't get Footsteps Component from %s."), *GetName(), *Owner->GetName());
		return;
	}

	// Okay so now we are finally doing the trace

	FVector Start = Owner->GetActorLocation();
	FVector End = Start - FVector(0, 0, 150.0f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);
	Params.bReturnPhysicalMaterial = true;

	FHitResult HitResult;

	// If successful hit
	if (World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		const EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
		FootstepsComp->PlaySoundBasedOnPhysSurface(SurfaceType);
	}
}
