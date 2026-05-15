#include "G2IAudioZoneComponent.h"
#include "G2I.h"
#include "Components/PrimitiveComponent.h"
#include "G2ISoundComponent.h"
#include "LaunchingIndication/G2ILauncherComponent.h"

UG2IAudioZoneComponent::UG2IAudioZoneComponent()
{
	SoundComp = CreateDefaultSubobject<UG2ISoundComponent>(TEXT("SoundComponent"));

	if (!ensure(SoundComp)) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't create SoundComponent in %s"), *GetNameSafe(GetOwner()));
		return;
	}
	else {
		SoundComp->SetupSounds.Add(PlayingSoundName, FSoundConfig());
	}

	LauncherComp = CreateDefaultSubobject<UG2ILauncherComponent>(TEXT("LauncherComp"));
	if (!ensure(LauncherComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create %s"), *GetNameSafe(GetOwner()),
			*UG2ILauncherComponent::StaticClass()->GetName());
	}
}

void UG2IAudioZoneComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner()) {
		AudioZoneTriggerCollision = Cast<UPrimitiveComponent>(AudioZoneTriggerCollisionRef.GetComponent(Owner));
		if (!ensure(AudioZoneTriggerCollision))
		{
			UE_LOG(LogG2I, Error, TEXT("UG2IAudioZoneComponent on %s: TriggerCollision isn't set"), *GetNameSafe(Owner));
		}
		else
		{
			AudioZoneTriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &UG2IAudioZoneComponent::OnOverlapBegin);
			AudioZoneTriggerCollision->OnComponentEndOverlap.AddDynamic(this, &UG2IAudioZoneComponent::OnOverlapEnd);
		}
	}
	else {
		UE_LOG(LogG2I, Error, TEXT("UG2IAudioZoneComponent: can't get Owner. Component name: %s"), *GetNameSafe(Owner));
	}

	if (!ensure(SoundComp))
	{
		UE_LOG(LogG2I, Error, TEXT("SoundComponent doesn't exist in %s"), *GetNameSafe(GetOwner()));
		return;
	}

	if (const auto* SoundConf = SoundComp->SetupSounds.Find(PlayingSoundName)) {
		PlayingSoundId = SoundComp->AddSound(*SoundConf);
	}
}

void UG2IAudioZoneComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!ensure(LauncherComp)) {
		UE_LOG(LogG2I, Error, TEXT("UG2IAudioZoneComponent on %s: can't get Launcher component"), *GetNameSafe(GetOwner()));
		return;
	}

	if (LauncherComp->IsLocked_Implementation()) {
		UE_LOG(LogG2I, Verbose, TEXT("UG2IAudioZoneComponent on %s: Launcher is locked"), *GetNameSafe(GetOwner()));
		return;
	}

	if (!ensure(OtherActor)) {
		UE_LOG(LogG2I, Error, TEXT("UG2IAudioZoneComponent on %s: can't get Other Actor"), *GetNameSafe(GetOwner()));
		return;
	}

	if (!AllowedActors.Contains(OtherActor))
	{
		return;
	}

	if (!ensure(SoundComp))
	{
		UE_LOG(LogG2I, Error, TEXT("SoundComponent doesn't exist in %s"), *GetNameSafe(GetOwner()));
		return;
	}

	SoundComp->PlaySound(PlayingSoundId);
}

void UG2IAudioZoneComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bIsStopWhenLeaving) {
		return;
	}

	if (!ensure(OtherActor)) {
		UE_LOG(LogG2I, Error, TEXT("UG2IAudioZoneComponent on %s: can't get Other Actor"), *GetNameSafe(GetOwner()));
		return;
	}

	if (!AllowedActors.Contains(OtherActor))
	{
		return;
	}

	if (!ensure(SoundComp))
	{
		UE_LOG(LogG2I, Error, TEXT("SoundComponent doesn't exist in %s"), *GetNameSafe(GetOwner()));
		return;
	}

	SoundComp->StopSound(PlayingSoundId);
}

void UG2IAudioZoneComponent::OnRegister()
{
	Super::OnRegister();

	if (!ensure(LauncherComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find PhysicsHandleComponent"), *GetName());
	}
	else
	{
		LauncherComp->RegisterComponent();
	}

	if (!ensure(SoundComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find SoundComp"), *GetName());
	}
	else
	{
		SoundComp->RegisterComponent();
	}
}