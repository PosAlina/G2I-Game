#include "Gameplay/G2IMovingBySteamAndHandsObject.h"
#include "G2I.h"
#include "G2IOutlineComponent.h"
#include "Components/SteamGlove/G2ISteamGloveComponent.h"
#include "G2ISoundComponent.h"

AG2IMovingBySteamAndHandsObject::AG2IMovingBySteamAndHandsObject()
{
	Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("SteamForceTimeline"));
	TimelineValue = 0.0f;
	TimelineCurve = nullptr;
	
	OutlineComponent = CreateDefaultSubobject<UG2IOutlineComponent>(TEXT("OutlineComponent"));
	if (!ensure(OutlineComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Failed to create Outline Component"), *GetName());
	}

	SoundComp = CreateDefaultSubobject<UG2ISoundComponent>(TEXT("SoundComponent"));
	if (SoundComp) {
		SoundComp->SetupSounds.Add(BounceSoundName, FSoundConfig());
	}
}

void AG2IMovingBySteamAndHandsObject::BeginPlay()
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

	if (!ensure(SoundComp)) {
		UE_LOG(LogG2I, Warning, TEXT("Sound Component is not set for %s"), *GetName());
		return;
	}

	if (SoundComp->SetupSounds.Contains(BounceSoundName))
	{
		BounceSoundId = SoundComp->AddSound(SoundComp->SetupSounds[BounceSoundName]);
		if (BounceSoundId == -1)
		{
			UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: BounceSoundId (ID == -1)"), *GetName(), *FString(__FUNCTION__));
		}
	}
}


void AG2IMovingBySteamAndHandsObject::OnTimelineUpdate(const float Output)
{
	UStaticMeshComponent* SM = this->GetComponentByClass<UStaticMeshComponent>();
	if (!ensure(SM)) {
		UE_LOG(LogG2I, Warning, TEXT("Actor %s don't have static mesh"), *GetName());
		Timeline->Stop();
		return;
	}

	const FVector MoveDelta = ForwardVector * Output * SteamPushForce * Multiplier;
	const FVector TargetLocation = SM->GetComponentLocation() + MoveDelta + FVector(0.f, 0.f, 0.001f);

	FHitResult SweepResult;
	SM->SetWorldLocation(TargetLocation, true, &SweepResult);

	if (!SweepResult.bBlockingHit) {
		return;
	}

	const FVector ImpactNormal = SweepResult.ImpactNormal.GetSafeNormal();

	if (ImpactNormal.Z >= 0.9f) {
		return;
	}
	ForwardVector = FMath::GetReflectionVector(ForwardVector, ImpactNormal);
	ForwardVector.Normalize(); 

	Multiplier *= BounceMultiplier;

	const FVector BounceOffset = ForwardVector;
	const FVector NewLocation = SM->GetComponentLocation() + BounceOffset;

	SM->SetWorldLocation(NewLocation, false);
	UG2ISoundComponent::PlaySoundSafe(SoundComp, BounceSoundId);
}

bool AG2IMovingBySteamAndHandsObject::CanInteract_Implementation(const ACharacter* Interactor) {

	if (!Interactor) {
		UE_LOG(LogG2I, Error, TEXT("Interactor not founded"));
		return false;
	}
	return PossibleInteractors.Contains(Interactor);
}

void AG2IMovingBySteamAndHandsObject::Interact_Implementation(const ACharacter* Interactor)
{
	//TODO Later
	/*if (!Interactor) {
		UE_LOG(LogG2I, Error, TEXT("Interactor not founded"));
		return;
	}
	if (!PushingConstraint) {
		UE_LOG(LogG2I, Error, TEXT("Physics Constraint not founded"));
		return;
	}
	
	UCapsuleComponent* CharacterCapsule = Interactor->GetCapsuleComponent();
	if (!CharacterCapsule)
	{
		UE_LOG(LogG2I, Error, TEXT("%s has no capsule component"), *Interactor->GetName());
		return;
	}

	UPrimitiveComponent* PrimitiveCapsule = Cast<UPrimitiveComponent>(CharacterCapsule);
	if (!PrimitiveCapsule)
	{
		UE_LOG(LogG2I, Error, TEXT("Failed to cast capsule to primitive component"));
		return;
	}

	UStaticMeshComponent* ThisStaticMesh = FindComponentByClass<UStaticMeshComponent>();
	if (!ThisStaticMesh)
	{
		UE_LOG(LogG2I, Error, TEXT("%s has no StaticMeshComponent"), *this->GetName());
		return;
	}

	PushingConstraint->SetConstrainedComponents(
													PrimitiveCapsule,
													NAME_None,             
													ThisStaticMesh,        
													NAME_None               
												);
	PushingConstraint->SetDisableCollision(true);
	PushingConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.0f);
	PushingConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0.0f);
	PushingConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0.0f);*/
}

void AG2IMovingBySteamAndHandsObject::OnShoot_Implementation(const FHitResult& HitResult, AActor* Character)
{
	if (!Character) {
		UE_LOG(LogG2I, Error, TEXT("Character not founded"));
		return;
	}

	if (!Character->FindComponentByClass<UG2ISteamGloveComponent>()) {
		UE_LOG(LogG2I, Warning, TEXT("Character %s don't have steam glove"), *Character->GetName());
		return;
	}

	ForwardVector = HitResult.Location - HitResult.TraceStart;
	ForwardVector.Z = 0;
	ForwardVector.Normalize();
	Multiplier = 1.0f;

	if (!Timeline) {
		UE_LOG(LogG2I, Error, TEXT("Can't start timeline for %s"), *GetName());
		return;
	}

	Timeline->Stop();
	Timeline->PlayFromStart();
}

float AG2IMovingBySteamAndHandsObject::GetSpeedChange_Implementation()
{
	return 0.0f;//TODO Later
}
