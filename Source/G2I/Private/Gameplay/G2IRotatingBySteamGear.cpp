#include "Gameplay/G2IRotatingBySteamGear.h"
#include "G2I.h"
#include "G2IOutlineComponent.h"
#include "Interfaces/G2IMovingByGearObjectInterface.h"
#include "Components/G2IInventoryComponent.h"
#include "EngineUtils.h"
#include "Sound/G2ISoundComponent.h"

AG2IRotatingBySteamGear::AG2IRotatingBySteamGear()
{
	Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("SteamForceTimeline"));
	TimelineValue = 0.0f;
	TimelineCurve = nullptr;

	SoundComp = CreateDefaultSubobject<UG2ISoundComponent>(TEXT("SoundComponent"));
	if (SoundComp) {
		SoundComp->SetupAttachment(RootComponent);
		SoundComp->SetupSounds.Add(GearRotationSoundName, FSoundConfig());
		SoundComp->SetupSounds.Add(ActorMovingWithSplineSoundName, FSoundConfig());
	}
	OutlineComponent = CreateDefaultSubobject<UG2IOutlineComponent>(TEXT("OutlineComponent"));
	if (!ensure(OutlineComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Failed to create Outline Component"), *GetName());
	}
}

void AG2IRotatingBySteamGear::BeginPlay()
{
	Super::BeginPlay();

	if (!TimelineCurve) {
		UE_LOG(LogG2I, Warning, TEXT("TimelineCurve is not set for %s"), *GetName());
		return;
	}
	TimelineUpdate.BindUFunction(this, FName("OnTimelineUpdate"));
	TimelineFinished.BindUFunction(this, FName("OnTimelineFinished"));
	if (!Timeline) {
		UE_LOG(LogG2I, Warning, TEXT("Timeline is not exist for %s"), *GetName());
		return;
	}
	Timeline->AddInterpFloat(TimelineCurve, TimelineUpdate);
	Timeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
	Timeline->SetTimelineFinishedFunc(TimelineFinished);
	Timeline->SetLooping(false);

	if (!ensure(SoundComp)) {
		UE_LOG(LogG2I, Warning, TEXT("Sound Component is not set for %s"), *GetName());
		return;
	}

	if (SoundComp->SetupSounds.Contains(GearRotationSoundName))
	{
		GearRotationSoundId = SoundComp->AddSound(SoundComp->SetupSounds[GearRotationSoundName]);
		if (GearRotationSoundId == -1)
		{
			UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: GearRotationSoundId (ID == -1)"), *GetName(), *FString(__FUNCTION__));
		}
	}

	if (SoundComp->SetupSounds.Contains(ActorMovingWithSplineSoundName))
	{
		ActorMovingWithSplineSoundId = SoundComp->AddSound(SoundComp->SetupSounds[ActorMovingWithSplineSoundName]);
		if (ActorMovingWithSplineSoundId == -1)
		{
			UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ActorMovingWithSplineSoundId (ID == -1)"), *GetName(), *FString(__FUNCTION__));
		}
	}

	if (const UWorld* World = GetWorld()) {
		for (TActorIterator<AActor> It(GetWorld()); It; ++It)
		{
			AActor* CurrentActor = *It;
			if (CurrentActor->ActorHasTag(RotationTag))
			{
				MovableObjects.Add(CurrentActor);
			}
		}
	}
}

void AG2IRotatingBySteamGear::OnShoot_Implementation(const FHitResult& HitResult, AActor* Character)
{
	if (!IsActive) return;

	if (SoundComp) {
		SoundComp->SetSoundVolume(GearRotationSoundId, 1.0f);
		SoundComp->PlaySound(GearRotationSoundId);
		SoundComp->PlaySound(ActorMovingWithSplineSoundId);
	}

	const FVector HitDirection = (HitResult.ImpactPoint - HitResult.TraceStart).GetSafeNormal();

	const FVector LeverArm = HitResult.ImpactPoint - GetActorLocation();

	const FVector SimulatedTorque = FVector::CrossProduct(LeverArm, HitDirection);

	FVector AllowedRotationAxis = FVector::ZeroVector;
	if (bRotateRoll)  AllowedRotationAxis += GetActorForwardVector();
	if (bRotatePitch) AllowedRotationAxis += GetActorRightVector();
	if (bRotateYaw)   AllowedRotationAxis += GetActorUpVector();
	AllowedRotationAxis.Normalize();

	const float TorqueOnAxis = FVector::DotProduct(SimulatedTorque, AllowedRotationAxis);

	RotationSign = FMath::Sign(TorqueOnAxis);

	if (!Timeline) {
		UE_LOG(LogG2I, Error, TEXT("Can't start timeline for %s"), *GetName());
		return;
	}

	if (!Timeline->IsPlaying())
	{
		Timeline->PlayFromStart();
	}

	OnStartRotateDelegate.Broadcast(this);
}

void AG2IRotatingBySteamGear::OnTimelineUpdate(const float Output)
{
	UWorld* World = GetWorld();
	if (!ensure(World)) {
		UE_LOG(LogG2I, Error, TEXT("%s: Can't get the World"), *GetName());
		return;
	}

	const float DeltaTime = World->GetDeltaSeconds();
	const float CurrentRotationStep = Output * RotationSpeed * DeltaTime;

	for (const auto& i : MovableObjects) {
		if (!i) {
			UE_LOG(LogG2I, Error, TEXT("Can't get object to push for %s"), *GetName());
			continue;
		}

		if (i->Implements<UG2IMovingByGearObjectInterface>()) {
			IG2IMovingByGearObjectInterface::Execute_OnPushing(i, CurrentRotationStep * RotationSign);
		}
	}

	FRotator ActorRotator = FRotator::ZeroRotator;
	float StepWithSign = CurrentRotationStep * RotationSign;

	if (bRotateRoll) {
		ActorRotator.Roll = StepWithSign;
	}
	if (bRotatePitch) {
		ActorRotator.Pitch = StepWithSign;
	}
	if (bRotateYaw) {
		ActorRotator.Yaw = StepWithSign;
	}

	if (SoundComp) {
		SoundComp->SetSoundVolume(GearRotationSoundId, Output);
	}

	AddActorLocalRotation(ActorRotator);
}

void AG2IRotatingBySteamGear::Repair(AActor* Interactor)
{
	if (!Interactor)
		return;

	UG2IInventoryComponent* Inventory = Interactor->FindComponentByClass<UG2IInventoryComponent>();

	if (!Inventory)
		return;

	if (Inventory->HasItemID(RequiredItemID))
	{
		IsActive = true;

		Inventory->RemoveItemID(RequiredItemID);
	}
}

void AG2IRotatingBySteamGear::OnTimelineFinished()
{
	if (SoundComp) {
		SoundComp->StopSound(GearRotationSoundId);
		SoundComp->StopSound(ActorMovingWithSplineSoundId);
	}
}