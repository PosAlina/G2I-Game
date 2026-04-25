#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "G2IHandsMovableComponent.generated.h"

USTRUCT(BlueprintType)
struct FG2IDefaultParametersOfPrimitiveComponent
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> PrimitiveComponent;
	
	bool bIsSimulatePhysics = false;
	
	ECollisionChannel CollisionObjectType;
	
	FName CollisionProfileName;
	
	friend uint32 GetTypeHash(const FG2IDefaultParametersOfPrimitiveComponent& Reference)
	{
		return GetTypeHash(Reference.PrimitiveComponent);
	}
	bool operator==(const FG2IDefaultParametersOfPrimitiveComponent& Reference) const
	{
		return PrimitiveComponent == Reference.PrimitiveComponent;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class G2I_API UG2IHandsMovableComponent : public UPhysicsConstraintComponent
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere)
	TSet<TObjectPtr<APawn>> BanCharacters;
	
	UPROPERTY(EditAnywhere)
	bool bIsOverrideMass = false;
	
	UPROPERTY(EditAnywhere)
	float OverridenMass = 100.f;
	
	UPROPERTY(EditAnywhere)
	bool bIsOverrideLocationInHands = false;
	
	UPROPERTY(EditAnywhere)
	FVector OverridenLocationInHands = FVector(100.f, 0.f, 200.f);
	
	UPROPERTY(EditAnywhere)
	bool bIsOverrideRotationInHands = false;
	
	UPROPERTY(EditAnywhere)
	FRotator OverridenRotationInHands = FRotator(0.f, 0.f, 0.f);
	
protected:
	
	UPROPERTY()
	TSet<FG2IDefaultParametersOfPrimitiveComponent> PrimitiveComponents;
	
private:
	
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;
	
	UPROPERTY()
	TObjectPtr<AActor> Owner;

public:
	
	virtual void BeginPlay() override;
	
	bool CanMove() const;
	void PrepareBeforeAttachedPickedUp();
	void PrepareBeforePutDown();
	void PrepareBeforeGrabPickedUp();
	
private:
	
	void RememberComponentInfo(UPrimitiveComponent& Component);
	static void PrepareOneComponentBeforeGrabPickedUp(UPrimitiveComponent& Component);
	static void PrepareOneComponentBeforeAttachedPickedUp(UPrimitiveComponent& Component);
};
