#pragma once

#include "CoreMinimal.h"
#include "NavModifierComponent.h"
#include "Components/ActorComponent.h"
#include "G2IPassThroughComponent.generated.h"

/**
 * Component of actor that the daughter can pass through
 */
UCLASS(ClassGroup=(CollisionAndAINavigation), meta=(BlueprintSpawnableComponent))
class G2I_API UG2IPassThroughComponent : public UNavModifierComponent
{
	GENERATED_BODY()

public:
	
	UG2IPassThroughComponent();

protected:
	
	virtual void OnRegister() override;

private:

	void PreInitializeDefaults();
};
