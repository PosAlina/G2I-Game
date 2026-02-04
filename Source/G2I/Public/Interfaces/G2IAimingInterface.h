#pragma once

#include "CoreMinimal.h"
#include "G2IHitInfo.h"
#include "UObject/Interface.h"
#include "G2IAimingInterface.generated.h"

UINTERFACE()
class UG2IAimingInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface includes methods that are called:
 * - for aiming actions from input handlers
 * - for info about aiming for shooting
 */
class G2I_API IG2IAimingInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void StartAimingAction();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void StopAimingAction();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = State)
	bool IsAiming();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Getters)
	FG2IHitInfo GetAimLineHitInfo();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Getters)
	UActorComponent *GetCurrentComponentUsingAim();
	
};
