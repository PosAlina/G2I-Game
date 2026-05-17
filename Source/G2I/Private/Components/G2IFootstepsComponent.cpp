#include "Components/G2IFootstepsComponent.h"
#include "G2ISoundComponent.h"
#include "G2I.h"

UG2IFootstepsComponent::UG2IFootstepsComponent()
{
	SoundsForSurfaces.Add(SurfaceType1, TEXT("FootstepsWood"));
	SoundsForSurfaces.Add(SurfaceType2, TEXT("FootstepsMetal"));
	SoundsForSurfaces.Add(SurfaceType3, TEXT("FootstepsCarpet"));
	SoundsForSurfaces.Add(SurfaceType_Default, TEXT("FootstepsDefault"));
}

void UG2IFootstepsComponent::PlaySoundBasedOnPhysSurface(const EPhysicalSurface& SurfaceType)
{
	if (!SoundComp)
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: SoundComponent is null."), *GetName());
		return;
	}

	if (const FName* SoundName = SoundsForSurfaces.Find(SurfaceType))
	{
		if (const int32* SoundId = SoundIDs.Find(*SoundName))
		{
			SoundComp->PlaySound(*SoundId);
			return;
		}
	}

	const FName* SoundName = SoundsForSurfaces.Find(SurfaceType_Default);
	if (!SoundName)
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Couldn't find sound name for SurfaceType_Default."), *GetName());
		return;
	}

	if (const int32* SoundId = SoundIDs.Find(*SoundName))
	{
		SoundComp->PlaySound(*SoundId);
	}
}

void UG2IFootstepsComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Owner is null."), *GetName());
		return;
	}

	SoundComp = OwnerActor->FindComponentByClass<UG2ISoundComponent>();
	if (!SoundComp)
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Couldn't get %s from %s."), *GetName(), *UG2ISoundComponent::StaticClass()->GetName(), *OwnerActor->GetName());
		return;
	}

	// Setting up all the sounds
	for (const auto& [_, SoundName] : SoundsForSurfaces)
	{
		if (SoundComp->SetupSounds.Contains(SoundName))
		{
			const int32 NewSoundId = SoundComp->AddSound(SoundComp->SetupSounds[SoundName]);
			if (NewSoundId == -1)
			{
				UE_LOG(LogG2I, Warning, TEXT("%s: %s (ID == -1)."), *GetName(), *SoundName.ToString());
				continue;
			}
			SoundIDs.Add(SoundName, NewSoundId);
		}
	}
}
