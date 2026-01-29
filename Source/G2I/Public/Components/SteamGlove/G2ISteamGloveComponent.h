#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "G2ISteamGloveComponent.generated.h"

class UG2ISteamShotComponent;
class UG2IAimingComponent;
class UG2ISteamMovementComponent;
class UG2IReloadingComponent;

UCLASS(ClassGroup=(SteamGlove), meta=(BlueprintSpawnableComponent))
class G2I_API UG2ISteamGloveComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gadgets)
	TObjectPtr<UG2IReloadingComponent> ReloadingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gadgets)
	TObjectPtr<UG2IAimingComponent> SteamGloveAimingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gadgets)
	TObjectPtr<UG2ISteamShotComponent> SteamShotComp;

protected:
	/**
	 * TODO: These variables will be used in the future
	 *  to specify which bones
	 *  scene components of the steam glove
	 *  should be attached to.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attachment)
	FName AttachBoneName = FName("HandGrip_R");

	/**
	 * TODO: The point where the shot starts should be close to the aim to make it look more natural.
	 *  We'll need to clarify this when the animation is added.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attachment)
	FName AttachBoneNameShotComponent = FName("upperarm_r");
	
public:
	
	UG2ISteamGloveComponent();
	
	virtual void OnRegister() override;

private:
	
	void AttachGloveComponents();

	void AttachToOwnerRootComponent(USceneComponent* AttachableComponent);

};
