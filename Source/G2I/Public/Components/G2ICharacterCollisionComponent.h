#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "G2ICharacterCollisionComponent.generated.h"

class ACharacter;
class UCapsuleComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCharacterCollisionBeginOverlap, AActor *, OtherActor,
	UPrimitiveComponent *, OtherComp);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCharacterCollisionEndOverlap, AActor *, OtherActor,
	UPrimitiveComponent *, OtherComp);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class G2I_API UG2ICharacterCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FCharacterCollisionBeginOverlap OnCharacterCollisionBeginOverlap;

	UPROPERTY(BlueprintAssignable)
	FCharacterCollisionEndOverlap OnCharacterCollisionEndOverlap;

protected:
	
	static inline ECollisionChannel PassingThroughCharacterCollision = ECC_GameTraceChannel2;
	
private:

	UPROPERTY()
	TObjectPtr<ACharacter> Owner;

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> CollisionComponent;
	
protected:

	virtual void BeginPlay() override;

	virtual void OnRegister() override;

public:

	UPrimitiveComponent* GetCollisionComponent();

	static ECollisionChannel GetPassingThroughCollisionChannel();

private:

	UFUNCTION()
	void OnCapsuleBeginOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
		UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetupPassingThroughObjectCollision();

	void BindingToDelegates();

	void PreInitializeDefaults();
};
