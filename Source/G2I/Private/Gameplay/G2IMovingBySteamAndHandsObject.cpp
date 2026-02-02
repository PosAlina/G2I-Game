#include "Gameplay/G2IMovingBySteamAndHandsObject.h"
#include "G2I.h"
#include "Components/SteamGlove/G2ISteamGloveComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h" 
#include "Components/CapsuleComponent.h"

AG2IMovingBySteamAndHandsObject::AG2IMovingBySteamAndHandsObject()
{
	PrimaryActorTick.bCanEverTick = false;

	Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("SteamForceTimeline"));
	TimelineValue = 0.0f;
	TimelineCurve = nullptr;
}

void AG2IMovingBySteamAndHandsObject::BeginPlay()
{
	Super::BeginPlay();

	if (!TimelineCurve) {
		UE_LOG(LogG2I, Warning, TEXT("TimelineCurve is not set for %s"), *GetName());
		return;
	}
	TimelineUpdate.BindUFunction(this, FName("OnTimelineUpdate"));

	Timeline->AddInterpFloat(TimelineCurve, TimelineUpdate);
	Timeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
	Timeline->SetLooping(false);
}


void AG2IMovingBySteamAndHandsObject::OnTimelineUpdate(float Output)
{
	UStaticMeshComponent* SM = this->GetComponentByClass<UStaticMeshComponent>();
	if (!SM) {
		UE_LOG(LogG2I, Warning, TEXT("Actor %s don't have static mesh"), *GetName());
		Timeline->Stop();
		return;
	}

	FVector DeltaVector = ForwardVector * Output * SteamPushForce * Multiplier;
	DeltaVector += SM->GetComponentLocation();
	FVector SmallAxisZ = { 0.0f, 0.0f, 0.1f };
	DeltaVector += SmallAxisZ;

	FHitResult SweepResult;
	SM->SetWorldLocation(DeltaVector, true, &SweepResult);

	if (!SweepResult.bBlockingHit) {
		return;
	}

	SweepResult.ImpactNormal.Normalize();
	SmallAxisZ.Normalize();

	if (SweepResult.ImpactNormal.Z * SmallAxisZ.Z >= 0.9f) {
		return;
	}

	Multiplier *= -1 * BounceMultiplier;
	DeltaVector = ForwardVector * 10.0f * SteamPushForce * Multiplier;
	DeltaVector += SM->GetComponentLocation();
	SM->SetWorldLocation(DeltaVector, false, &SweepResult);
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
	}

	if (!Character->FindComponentByClass<UG2ISteamGloveComponent>()) {
		UE_LOG(LogG2I, Warning, TEXT("Character %s don't have steam glove"), *Character->GetName());
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
