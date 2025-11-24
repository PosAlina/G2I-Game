#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IInteractiveObjectInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UG2IInteractiveObjectInterface : public UInterface
{
	GENERATED_BODY()
};

class G2I_API IG2IInteractiveObjectInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactions")
	bool CanInteract(AActor* Interactor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactions")
	void Interact(AActor* Interactor);

};