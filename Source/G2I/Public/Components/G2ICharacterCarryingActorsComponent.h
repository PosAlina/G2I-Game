#pragma once

#include "CoreMinimal.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "G2ICharacterCarryingActorsComponent.generated.h"


class UG2IHandsMovableComponent;
class UCapsuleComponent;
class UBoxComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class G2I_API UG2ICharacterCarryingActorsComponent : public UPhysicsHandleComponent
{
	GENERATED_BODY()
	
public:
	
	bool bHandsAreBusy = false;
	
protected:
	
	UPROPERTY(EditAnywhere)
	FName AttachBoneName = FName("spline_001");
	
	UPROPERTY()
	TObjectPtr<UCapsuleComponent> Collider;
	FRotator OwnerRotation = FRotator(0.f, 0.f, 0.f);
	
	UPROPERTY()
	TObjectPtr<UBoxComponent> ColliderBox;
	
	UPROPERTY()
	TObjectPtr<AActor> GrabbedActor;
	
	UPROPERTY()
	TObjectPtr<UG2IHandsMovableComponent> HandsMovableComponent;
	
private:
	
	UPROPERTY()
	TObjectPtr<ACharacter> Owner;
	
	UPROPERTY()
	TObjectPtr<UCapsuleComponent>OwnerCapsule;
	
public:
	// Sets default values for this component's properties
	UG2ICharacterCarryingActorsComponent();

	bool PickUp(AActor& TargetActor, UG2IHandsMovableComponent& InHandsMovableComponent, bool bIsOverrideMass = true, float OverridenMass = 100.f,
	            bool bIsOverrideLocationInHands = false,
	            const FVector& OverridenLocationInHands = FVector(100.f, 0.f, 200.f),
	            bool bIsOverrideRotationInHands = false,
	            const FRotator& OverridenRotationInHands = FRotator(0.f, 0.f, 0.f));
	
	bool PutDown();
	
	// Called when the game starts
	virtual void BeginPlay() override;
	
protected:
	
	virtual void TickComponent(
		float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	
	void SetCollisionForCharacterWthGrabbedActor(const UPrimitiveComponent& ActorPrimitive) const;
};
