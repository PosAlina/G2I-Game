#include "Gameplay/G2ICoal.h"
#include "Components/G2IDestructibleComponent.h"
#include "G2I.h"
#include "GameFramework/Actor.h"
#include "G2ISoundComponent.h"

AG2ICoal::AG2ICoal() {
	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetGenerateOverlapEvents(true);

	DestructComp = CreateDefaultSubobject<UG2IDestructibleComponent>(TEXT("DestructibleComponent"));

	if (SceneRootComponent)
	{
		SetRootComponent(SceneRootComponent);
		if (StaticMeshComponent) {
			StaticMeshComponent->SetupAttachment(RootComponent);
		}
	}
	SoundComp = CreateDefaultSubobject<UG2ISoundComponent>(TEXT("CoalSoundComponent"));
	if (SoundComp) {
		SoundComp->SetupSounds.Add(FurnaceDestroySoundName, FSoundConfig());
		SoundComp->SetupSounds.Add(VoiceHitSoundName, FSoundConfig());
		SoundComp->SetupSounds.Add(HitDestroySoundName, FSoundConfig());
		SoundComp->SetupSounds.Add(VoiceFurnaceSoundName, FSoundConfig());
	}

	Tags.AddUnique(FName("Coal"));
	Tags.AddUnique(FName("NotGrabbedOneTime"));
}

void AG2ICoal::Deactivate_Implementation() {
	return;
}

void AG2ICoal::Activate_Implementation() {
	if (!ensure(DestructComp)) {
		UE_LOG(LogG2I, Warning, TEXT("%s couldn't get Destructible Component"), *GetName());
		return;
	}
	if (!ensure(SoundComp))
	{
		UE_LOG(LogG2I, Error, TEXT("SoundComponent doesn't exist in %s"), *GetName());
		return;
	}
	SoundComp->PlaySound(FurnaceDestroySoundId);
	SoundComp->PlaySound(VoiceFurnaceSoundId);
	DestructComp->DestroyOwner();
}

void AG2ICoal::BeginPlay() 
{
	Super::BeginPlay();

	if (StaticMeshComponent) {
		StaticMeshComponent->OnComponentHit.AddDynamic(this, &AG2ICoal::OnHit);
	}
	if (!ensure(SoundComp))
	{
		UE_LOG(LogG2I, Error, TEXT("SoundComponent doesn't exist in %s"), *GetName());
	}
	else {
		const auto* FurnaceDestroySoundConf = SoundComp->SetupSounds.Find(FurnaceDestroySoundName);
		const auto* HitDestroySoundConf = SoundComp->SetupSounds.Find(HitDestroySoundName);
		const auto* VoiceHitSoundConf = SoundComp->SetupSounds.Find(VoiceHitSoundName);
		const auto* VoiceFurnaceSoundConf = SoundComp->SetupSounds.Find(VoiceFurnaceSoundName);

		if (HitDestroySoundConf)
		{
			HitDestroySoundId = SoundComp->AddSound(*HitDestroySoundConf);
		}

		if (FurnaceDestroySoundConf)
		{
			FurnaceDestroySoundId = SoundComp->AddSound(*FurnaceDestroySoundConf);
		}

		if (VoiceFurnaceSoundConf) {
			VoiceFurnaceSoundId = SoundComp->AddSound(*VoiceFurnaceSoundConf);
		}
		if (VoiceHitSoundConf) {
			VoiceHitSoundId = SoundComp->AddSound(*VoiceHitSoundConf);
		}
	}
}


void AG2ICoal::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) 
{
	if (Tags.Contains(FName("NotGrabbedOneTime")))
	{
		UE_LOG(LogG2I, Verbose, TEXT("%s hasn't been grabbed yet "), *GetName());
		return;
	}

	if (!ensure(OtherActor)) {	
		UE_LOG(LogG2I, Error, TEXT("%s couldn't get Other Actor "), *GetName());
		return;
	}

	if (OtherActor->ActorHasTag(GrabberNameTag)) {
		UE_LOG(LogG2I, Verbose, TEXT("%s Other Actor has Grabber tag"), *GetName());
		return;
	}
	if (!ensure(StaticMeshComponent)) {
		UE_LOG(LogG2I, Error, TEXT("%s couldn't get Static Mesh Component "), *GetName());
		return;
	}
	UClass* CoalClass = GetClass();
	if (!ensure(CoalClass)) {
		UE_LOG(LogG2I, Warning, TEXT("%s couldn't get Class "), *GetName());
		return;
	}

	UWorld* World = GetWorld();
	if (!ensure(World)) {
		UE_LOG(LogG2I, Warning, TEXT("%s couldn't get World"), *GetName());
		return;
	}

	if (!ensure(SoundComp))
	{
		UE_LOG(LogG2I, Error, TEXT("SoundComponent doesn't exist in %s"), *GetName());
		return;
	}
	SoundComp->PlaySound(VoiceHitSoundId);
	SoundComp->PlaySound(HitDestroySoundId);

	StaticMeshComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
	StaticMeshComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

	StaticMeshComponent->SetWorldLocationAndRotation(CoalSpawnLocation, CoalSpawnRotation);
	Tags.AddUnique(FName("NotGrabbedOneTime"));
}