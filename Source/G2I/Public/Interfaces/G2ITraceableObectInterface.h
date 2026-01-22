

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2ITraceableObectInterface.generated.h"

UINTERFACE(MinimalAPI)
class UG2ITraceableObectInterface : public UInterface
{
	GENERATED_BODY()
};

class G2I_API IG2ITraceableObectInterface
{
	GENERATED_BODY()
public:

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Trace")
    void OnShoot(const FHitResult& HitResult, AActor* Character);

};
