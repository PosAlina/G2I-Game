#pragma once

#include "CoreMinimal.h"
#include "G2ILockingInterface.h"
#include "Components/ActorComponent.h"
#include "G2ILauncherComponent.generated.h"

class UG2IWorldHintKeyWidgetComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FLaunchDelegate,
                                               UG2ILauncherComponent*, LauncherComponent, AActor*, ComponentOwner, bool,
                                               bIsLaunched);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FLockedDelegate,
	UG2ILauncherComponent*, LauncherComponent, AActor*, ComponentOwner, bool, bIsLocked);

UCLASS(ClassGroup=(LaunchIndication), meta=(BlueprintSpawnableComponent))
class G2I_API UG2ILauncherComponent : public UActorComponent, public IG2ILockingInterface
{
	GENERATED_BODY()

private:

	bool bIsLaunched = false;

	UPROPERTY()
	FLaunchDelegate OnLaunchDelegate;

	bool bIsLocked = false;

	UPROPERTY()
	FLockedDelegate OnLockedDelegate;

	UPROPERTY()
	TObjectPtr<AActor> Owner;

	UPROPERTY()
	TObjectPtr<UG2IWorldHintKeyWidgetComponent>  HintKeyComp;
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLockAfterLaunch = true;

	UFUNCTION(BlueprintCallable)
	void SetIsLaunched(bool bNewIsLaunched);

	UFUNCTION(BlueprintCallable)
	bool IsLaunched() const;

	FLaunchDelegate& GetOnLaunchDelegate();

	UFUNCTION()
	virtual void SetIsLocked_Implementation(bool bNewIsLocked) override;
	
	UFUNCTION()
	virtual bool IsLocked_Implementation() override;

	void SetHintKeyWidget(UG2IWorldHintKeyWidgetComponent *InHintKeyWidget);

	FLockedDelegate& GetOnLockedDelegate();

protected:

	virtual void OnRegister() override;

	void DebugLockedMessage() const;
	void DebugLaunchedMessage() const;

};
