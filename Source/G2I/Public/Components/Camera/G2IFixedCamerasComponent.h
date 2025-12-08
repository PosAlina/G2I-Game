#pragma once

#include "CoreMinimal.h"
#include "Camera/G2ICameraInterface.h"
#include "Camera/G2IFixedCamerasInputInterface.h"
#include "Components/ActorComponent.h"
#include "G2IFixedCamerasComponent.generated.h"

class UG2ICharacterCollisionComponent;
class ACharacter;

UCLASS(ClassGroup=(Camera), meta=(BlueprintSpawnableComponent))
class G2I_API UG2IFixedCamerasComponent : public UActorComponent, public IG2ICameraInterface,
	public IG2IFixedCamerasInputInterface
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TObjectPtr<ACharacter> Owner;

	UPROPERTY()
	TObjectPtr<UG2ICharacterCollisionComponent> CollisionComponent;

	UPROPERTY(BlueprintAssignable)
	FAddCameraDelegate OnAddCameraDelegate;

	UPROPERTY(BlueprintAssignable)
	FRemoveCameraDelegate OnRemoveCameraDelegate;
	
	UPROPERTY()
	TArray<TObjectPtr<UCameraComponent>> CurrentFixedCameras;
	
protected:

	virtual void BeginPlay() override;

	virtual void OnRegister() override;

public:

	// Interface methods
	UFUNCTION(BlueprintCallable, Category="Camera")
	virtual TArray<UCameraComponent *> GetCameraComponents_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category="Camera")
	virtual void SetupCameras_Implementation() override;

	UFUNCTION()
	virtual FAddCameraDelegate& GetAddCameraDelegate() override;

	UFUNCTION()
	virtual FRemoveCameraDelegate& GetRemoveCameraDelegate() override;

private:

	UFUNCTION()
	void OnCharacterBeginOverlapTriggerZone(AActor *OtherActor, UPrimitiveComponent *OtherComp);

	UFUNCTION()
	void OnCharacterEndOverlapTriggerZone(AActor *OtherActor, UPrimitiveComponent *OtherComp);

	void PreInitializationDefaults();

	void SetupDefaults();
	
	void BindingToDelegates();

	void AddCamera(UCameraComponent& AddedCamera);

	void RemovedCamera(UCameraComponent& RemovedCamera);
	
};
