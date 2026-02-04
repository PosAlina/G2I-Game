#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IMechanicUsingAimInterface.generated.h"

enum class EG2IAimType : uint8;

UINTERFACE()
class UG2IMechanicUsingAimInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface for gameplay mechanics which using aiming
 */
class G2I_API IG2IMechanicUsingAimInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void ActivateForAim();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	EG2IAimType GetAimTypeByActor(const AActor *TargetActor);
};
