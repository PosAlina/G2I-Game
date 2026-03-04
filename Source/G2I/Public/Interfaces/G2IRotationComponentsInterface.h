

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IRotationComponentsInterface.generated.h"

UINTERFACE(MinimalAPI)
class UG2IRotationComponentsInterface : public UInterface
{
	GENERATED_BODY()
};

class G2I_API IG2IRotationComponentsInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ChangeAngle(const TArray<float>& Y, bool bIsOn);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Answer(bool bIsRight);
};
