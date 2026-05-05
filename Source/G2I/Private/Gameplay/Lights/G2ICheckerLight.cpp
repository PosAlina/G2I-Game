#include "Lights/G2ICheckerLight.h"
#include "G2I.h"
#include "G2ISoundComponent.h"
#include "LaunchingIndication/G2ILauncherComponent.h"

AG2ICheckerLight::AG2ICheckerLight()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	if (!ensure(RootComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create RootComponent"), *GetName());
		return;
	}
	
	SoundComp = CreateDefaultSubobject<UG2ISoundComponent>(TEXT("SoundComponent"));
	if (!ensure(SoundComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create SoundComponent"), *GetName());
		return;
	} 
	SoundComp->SetupSounds.Add(ActivateLampSoundName, FSoundConfig());
	SoundComp->SetupSounds.Add(DeactivateLampSoundName, FSoundConfig());
	
	LauncherComp = CreateDefaultSubobject<UG2ILauncherComponent>(TEXT("LauncherComp"));
	if (!ensure(LauncherComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create %s"), *GetActorNameOrLabel(),
			*UG2ILauncherComponent::StaticClass()->GetName());
	}
}

void AG2ICheckerLight::BeginPlay()
{
	Super::BeginPlay();
	
	if (bIsLampActivated)
	{
		SetVisualActivateLight();
	}
	else
	{
		SetVisualDeactivateLight();
	}
	
	AddSoundLamp(ActivateLampSoundName, ActivateSoundID);
	AddSoundLamp(DeactivateLampSoundName, DeactivateSoundID);
	bIsInitialized = true;
	BindDelegates();
}

void AG2ICheckerLight::BindDelegates()
{
	if (!ensure(LauncherComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*UG2ILauncherComponent::StaticClass()->GetName());
		return;
	}
	LauncherComp->GetOnLockedDelegate().AddDynamic(this, &ThisClass::Lock);
}

void AG2ICheckerLight::Activate_Implementation()
{
	if (!bIsInitialized)
	{
		return;
	}
	SetVisualActivateLight();
	if (bActivateSoundEnable)
	{
		PlaySound(ActivateSoundID);
	}
	bIsLampActivated = true;
}

void AG2ICheckerLight::Deactivate_Implementation()
{
	if (!bIsInitialized)
	{
		return;
	}
	SetVisualDeactivateLight();
	if (bDeactivateSoundEnable)
	{
		PlaySound(DeactivateSoundID);
	}
	bIsLampActivated = false;
}

void AG2ICheckerLight::AddSoundLamp(const FName& SoundName, int32& SoundID) const
{
	if (!ensure(SoundComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find SoundComponent"), *GetName());
		return;
	}
	const FSoundConfig* SoundConfig = SoundComp->SetupSounds.Find(SoundName);
	if (!ensure(SoundConfig))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Couldn't find sound with name %s"), *GetName(), *SoundName.ToString());
		return;
	}
	SoundID = SoundComp->AddSound(*SoundConfig);
	if (!ensure(SoundID != -1))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Couldn't add sound with name %s"), *GetName(), *SoundName.ToString());
		return;
	}
}

void AG2ICheckerLight::PlaySound(const int32& SoundID) const
{
	if (!ensure(SoundComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find SoundComponent"), *GetName());
		return;
	}
	if (!ensure(SoundComp->PlaySound(SoundID)))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Couldn't play sound with ID %i"), *GetName(), SoundID);
		return;
	}
}

void AG2ICheckerLight::Lock(UG2ILauncherComponent* LauncherComponent, AActor* ComponentOwner, const bool bIsLocked)
{
	if (bIsLocked && bIsLampActivated)
	{
		Deactivate_Implementation();
		return;
	}
	if (!bIsLocked && !bIsLampActivated)
	{
		Activate_Implementation();
		return;
	}
}
