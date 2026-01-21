#pragma once

#include "CoreMinimal.h"
#include "G2IHitInfo.h"
#include "UObject/Interface.h"
#include "G2ISteamShotInputInterface.generated.h"

UINTERFACE()
class UG2ISteamShotInputInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface includes methods that are called for shooting from steam glove
 */
class G2I_API IG2ISteamShotInputInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void ShootAction(const FG2IHitInfo TargetHitInfo);
};
