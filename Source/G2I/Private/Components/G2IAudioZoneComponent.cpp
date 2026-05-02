#include "G2IAudioZoneComponent.h"
#include "G2I.h"
#include "Components/PrimitiveComponent.h"
#include "G2ISoundComponent.h"

UG2IAudioZoneComponent::UG2IAudioZoneComponent()
{
	SoundComp = CreateDefaultSubobject<UG2ISoundComponent>(TEXT("SoundComponent"));

	if (!ensure(SoundComp)) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't create SoundComponent in %s"), *GetName());
		return;
	}
	else {
		SoundComp->SetupSounds.Add(PlayingSoundName, FSoundConfig());
	}
}

void UG2IAudioZoneComponent::BeginPlay()
{
	Super::BeginPlay();
	if (AActor* Owner = GetOwner()) {

		AudioZoneTriggerCollision = Cast<UPrimitiveComponent>(AudioZoneTriggerCollisionRef.GetComponent(Owner));
		if (!ensure(AudioZoneTriggerCollision))
		{

			UE_LOG(LogG2I, Error, TEXT("UG2IAudioZoneComponent on %s: TriggerCollision isn't set"), *Owner->GetName());
		}
		else
		{
			AudioZoneTriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &UG2IAudioZoneComponent::OnOverlapBegin);
			AudioZoneTriggerCollision->OnComponentEndOverlap.AddDynamic(this, &UG2IAudioZoneComponent::OnOverlapEnd);
		}
	}
	else {
		UE_LOG(LogG2I, Error, TEXT("UG2IAudioZoneComponent: can't get Owner"));
	}

	if (!ensure(SoundComp))
	{
		UE_LOG(LogG2I, Error, TEXT("SoundComponent doesn't exist in %s"), *GetName());
		return;
	}
	
	if (const auto* SoundConf = SoundComp->SetupSounds.Find(PlayingSoundName)) {
		PlayingSoundId = SoundComp->AddSound(*SoundConf);
	}
}

void UG2IAudioZoneComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!ensure(OtherActor)) {
		if (const AActor* Owner = GetOwner()) {
			UE_LOG(LogG2I, Error, TEXT("UG2IAudioZoneComponent on %s: can't get Other Actor"), *Owner->GetName());
		}
		else {
			UE_LOG(LogG2I, Error, TEXT("UG2IAudioZoneComponent: can't get Other Actor and can't get Owner"));
		}
	}

	if (!AllowedActors.Contains(OtherActor))
	{
		return;
	}

	if (!ensure(SoundComp))
	{
		UE_LOG(LogG2I, Error, TEXT("SoundComponent doesn't exist in %s"), *GetName());
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
		if (const AActor* Owner = GetOwner()) {
			UE_LOG(LogG2I, Error, TEXT("UG2IAudioZoneComponent on %s: can't get Other Actor"), *Owner->GetName());
		}
		else {
			UE_LOG(LogG2I, Error, TEXT("UG2IAudioZoneComponent: can't get Other Actor and can't get Owner"));
		}
	}

	if (!AllowedActors.Contains(OtherActor))
	{
		return;
	}

	if (!ensure(SoundComp))
	{
		UE_LOG(LogG2I, Error, TEXT("SoundComponent doesn't exist in %s"), *GetName());
		return;
	}

	SoundComp->StopSound(PlayingSoundId);
}