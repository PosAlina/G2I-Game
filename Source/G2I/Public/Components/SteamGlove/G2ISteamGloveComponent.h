#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "G2ISteamGloveComponent.generated.h"

class UG2ISteamShotComponent;
class UG2IAimingComponent;
class UG2IGlovePunchComponent;

#if WITH_EDITORONLY_DATA
class UG2ISteamMovementComponent;
class UG2IReloadingComponent;
#endif

UCLASS(ClassGroup=(SteamGlove), meta=(BlueprintSpawnableComponent))
class G2I_API UG2ISteamGloveComponent : public USceneComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gadgets)
	TObjectPtr<UG2IAimingComponent> SteamGloveAimingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gadgets)
	TObjectPtr<UG2ISteamShotComponent> SteamShotComp;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gadgets)
	TObjectPtr<UG2IReloadingComponent> ReloadingComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gadgets)
	TObjectPtr<UG2ISteamMovementComponent> SteamMovementComp;
#endif

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attachment)
	FName AttachBoneName = FName("hand_r");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attachment)
	FName AttachBoneNameShotComponent = FName("ring_metacarpal_r");
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gadgets)
	TObjectPtr<UG2IGlovePunchComponent> GlovePunchComp;
	
public:
	
	UG2ISteamGloveComponent();
	virtual void InitializeComponent() override;

private:
	
	void AttachGloveComponents();

	void AttachToOwnerRootComponent(USceneComponent* AttachableComponent);

};
