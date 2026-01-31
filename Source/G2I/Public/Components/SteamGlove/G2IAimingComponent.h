#pragma once

#include "CoreMinimal.h"
#include "G2IAimingInterface.h"
#include "G2IHitInfo.h"
#include "Components/ActorComponent.h"
#include "G2IAimTypeEnum.h"
#include "G2IAimingComponent.generated.h"

enum class EG2ICameraBlendState : uint8;
enum class EG2IAimType : uint8;
enum class EG2ICameraTypeEnum : uint8;
class UG2IAimingWidget;
class AG2IPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartAimingDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFinishAimingDelegate);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class G2I_API UG2IAimingComponent : public UActorComponent, public IG2IAimingInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FStartAimingDelegate OnStartAimingDelegate;

	UPROPERTY(BlueprintAssignable)
	FFinishAimingDelegate OnFinishAimingDelegate;
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = View)
	TSubclassOf<UG2IAimingWidget> AimWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UG2IAimingWidget> AimWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = View, meta = (Units = "s", ToolTip =
		"After the event, the aim takes on a specific view for that time. Afterward, the aiming logic returns"))
	float PendingAimViewMaxTime = 1.f;

	bool bAimViewIsPending = false;
	
	bool bIsAiming = false;

	bool bCanAiming = true;

	bool bWantsAiming = false;

private:

	UPROPERTY()
	TObjectPtr<UWorld> World;

	UPROPERTY()
	TObjectPtr<AG2IPlayerController> PlayerController;

	UPROPERTY()
	TArray<TObjectPtr<UActorComponent>> ComponentsUsingAim;

	int32 ComponentUsingAimIndex = 0;

	float CurrentAimDistance = FLT_MAX;

	FG2IHitInfo AimLineHitInfo;
	
	UPROPERTY()
	TObjectPtr<AActor> AimTargetActor;

	EG2IAimType CurrentAimType = EG2IAimType::DefaultAim;

	EG2IAimType PendingAimType = EG2IAimType::DefaultAim;
	
	float PendingAimViewElapsedTime = 0.f;

public:

	UG2IAimingComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
						   FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = Input)
	virtual void StartAimingAction_Implementation() override;
	
	UFUNCTION(BlueprintCallable, Category = Input)
	virtual void StopAimingAction_Implementation() override;
	
	UFUNCTION(BlueprintCallable, Category = State)
	virtual bool IsAiming_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = Getters)
	virtual FG2IHitInfo GetAimLineHitInfo_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = Getters)
	virtual UActorComponent *GetCurrentComponentUsingAim_Implementation() override;
		
protected:

	UFUNCTION()
	void SetAbilityAiming(EG2ICameraTypeEnum CurrentCameraType, EG2ICameraBlendState CurrentBlendState);
	
	UFUNCTION()
	void SetAimDistance(const float NewAimDistance);

	UFUNCTION()
	void SetPendingAimType(EG2IAimType NewAimType);

private:

	void SetupDefaults();
	
	void BindDelegates();

	void DetectAimLineHitInfo();

	void ActivateCurrentComponentUsingAim();

	void SetAimType(const AActor* TargetActor);

};