#pragma once

#include "CoreMinimal.h"
#include "G2IHitInfo.generated.h"

USTRUCT(BlueprintType)
struct FG2IHitInfo
{
	GENERATED_BODY()

	bool HitSuccess = false;
	FHitResult HitResult;
};