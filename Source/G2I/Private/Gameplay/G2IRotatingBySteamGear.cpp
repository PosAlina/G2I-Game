#include "Gameplay/G2IRotatingBySteamGear.h"
#include "G2I.h"
#include "G2IOutlineComponent.h"
#include "Interfaces/G2IMovingByGearObjectInterface.h"
#include "Components/G2IInventoryComponent.h"
#include "Sound/G2ISoundComponent.h"

AG2IRotatingBySteamGear::AG2IRotatingBySteamGear()
{
	Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("SteamForceTimeline"));
	TimelineValue = 0.0f;
	TimelineCurve = nullptr;

	SoundComp = CreateDefaultSubobject<UG2ISoundComponent>(TEXT("SoundComponent"));
	if (SoundComp) {
		SoundComp->SetupAttachment(RootComponent);
		SoundComp->SetupSounds.Add(TEXT("GearRotationSound"), FSoundConfig());
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

	if (!Timeline) {
		UE_LOG(LogG2I, Warning, TEXT("Timeline is not exist for %s"), *GetName());
		return;
	}
	Timeline->AddInterpFloat(TimelineCurve, TimelineUpdate);
	Timeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
	Timeline->SetLooping(false);

	if (SoundComp && SoundComp->SetupSounds.Contains(TEXT("GearRotationSound")))
	{
		GearRotationSoundId = SoundComp->AddSound(SoundComp->SetupSounds[TEXT("GearRotationSound")]);
		if (GearRotationSoundId == -1)
		{
			UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: GearRotationSoundId (ID == -1)"), *GetName(), *FString(__FUNCTION__));
		}
	}
}

void AG2IRotatingBySteamGear::OnShoot_Implementation(const FHitResult& HitResult, AActor* Character) {
	if (IsActive)
	{
		//Wrap into pushing with use of all axis
		//Calculating direction based on trace and rotation normal
		FVector HitVector = HitResult.ImpactPoint - HitResult.TraceStart;
		HitVector.X = HitVector.Z;
		HitVector.Z = 0;
		const FVector RotationNormal = { 1.0f, 0.0f, 0.0f };
		const float CosineValue = FVector::DotProduct(HitVector.GetSafeNormal(), RotationNormal.GetSafeNormal());

		//Calculating direction based on HitNormal and axis Z
		FVector HitNormal = HitResult.ImpactNormal;
		HitNormal.X = 0;
		const FVector AxisZ = { 0.0f, 0.0f, 1.0f };
		const FVector CrossProductResult = FVector::CrossProduct(AxisZ, HitNormal);

		RotationSign = CrossProductResult.X * CosineValue;

		if (!Timeline) {
			UE_LOG(LogG2I, Error, TEXT("Can't start timeline for %s"), *GetName());
			return;
		}

		if (!Timeline->IsPlaying())
		{
			Timeline->PlayFromStart();
		}
	}
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

	FRotator ActorRotator = { 0.0f, 0.0f, 0.0f };
	if (bRotateRoll) {
		ActorRotator.Roll = CurrentRotationStep * FMath::Sign(RotationSign);
	}
	if (bRotatePitch) {
		ActorRotator.Pitch = CurrentRotationStep * FMath::Sign(RotationSign);
	}
	if (bRotateYaw) {
		ActorRotator.Yaw = CurrentRotationStep * FMath::Sign(RotationSign);
	}
	AddActorWorldRotation(ActorRotator);
	if (SoundComp) {
		if (!SoundComp->IsSoundPlaying(GearRotationSoundId)) {
			SoundComp->PlaySound(GearRotationSoundId);
		}
	}
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