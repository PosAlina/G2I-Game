

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2ITraceableObectInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UG2ITraceableObectInterface : public UInterface
{
	GENERATED_BODY()
};

class G2I_API IG2ITraceableObectInterface
{
	GENERATED_BODY()
public:

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactions")
    void OnShoot(const FHitResult& HitResult, AActor* Character);

};
