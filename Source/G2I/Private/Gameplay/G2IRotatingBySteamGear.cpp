#include "Gameplay/G2IRotatingBySteamGear.h"
#include "G2I.h"
#include "Interfaces/G2IMovingByGearObjectInterface.h"

AG2IRotatingBySteamGear::AG2IRotatingBySteamGear()
{
	Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("SteamForceTimeline"));
	TimelineValue = 0.0f;
	TimelineCurve = nullptr;
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
}

void AG2IRotatingBySteamGear::OnShoot_Implementation(const FHitResult& HitResult, AActor* Character) {
	//Wrap into pushing with use of all axis
	//Calculating direction based on trace and rotation normal
	FVector HitVector = HitResult.ImpactPoint - HitResult.TraceStart;
	HitVector.X = HitVector.Z;
	HitVector.Z = 0;
	FVector RotationNormal = { 1.0f, 0.0f, 0.0f };
	float CosineValue = FVector::DotProduct(HitVector.GetSafeNormal(), RotationNormal.GetSafeNormal());

	//Calculating direction based on HitNormal and axis Z
	FVector HitNormal = HitResult.ImpactNormal;
	HitNormal.X = 0;
	FVector AxisZ = { 0.0f, 0.0f, 1.0f };
	FVector CrossProductResult = FVector::CrossProduct(AxisZ, HitNormal);

	RotationSign = CrossProductResult.X * CosineValue;

	if (!Timeline) {
		UE_LOG(LogG2I, Error, TEXT("Can't start timeline for %s"), *GetName());
		return;
	}

	Timeline->Stop();
	Timeline->PlayFromStart();
}

void AG2IRotatingBySteamGear::OnTimelineUpdate(float Output)
{
	for (const auto& i : MovableObjects) {
		if (!i) {
			UE_LOG(LogG2I, Error, TEXT("Can't get object to push for %s"), *GetName());
			continue;
		}

		if (i->Implements<UG2IMovingByGearObjectInterface>()) {
			IG2IMovingByGearObjectInterface::Execute_OnPushing(i, Output * RotationSpeed * RotationSign);
		}
	}

	FRotator ActorRotator = { 0.0f, 0.0f, 0.0f };
	if (bRotateRoll) {
		ActorRotator.Roll = Output * RotationSpeed * RotationSign;
	}
	if (bRotatePitch) {
		ActorRotator.Pitch = Output * RotationSpeed * RotationSign;
	}
	if (bRotateYaw) {
		ActorRotator.Yaw = Output * RotationSpeed * RotationSign;
	}
	AddActorWorldRotation(ActorRotator);
}