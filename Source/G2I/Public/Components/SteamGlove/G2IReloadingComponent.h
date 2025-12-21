#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "G2IReloadingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartReloadingDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFinishReloadingDelegate);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class G2I_API UG2IReloadingComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	
	UPROPERTY(BlueprintAssignable)
	FStartReloadingDelegate OnStartReloadingDelegate;

	UPROPERTY(BlueprintAssignable)
	FFinishReloadingDelegate OnFinishReloadingDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReloadTime = 4.f;

private:

	UPROPERTY()
	TObjectPtr<UWorld> World;

	UPROPERTY()
	FTimerHandle TimerHandle;
	
public:

	virtual void BeginPlay() override;

protected:

	UFUNCTION()
	void Reload();

private:
	
	void FinishReloading();

	void ShowReloadingOnView();

	void SetupDefaults();

	void BindDelegates();
	
};
