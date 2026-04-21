#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "G2IBlockWorldWidgetVisibility.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class G2I_API AG2IBlockWorldWidgetVisibility : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> CollisionComponent;
	
public:
	
	virtual void PreInitializeComponents() override;
	
protected:
	
	AG2IBlockWorldWidgetVisibility();
	
};
