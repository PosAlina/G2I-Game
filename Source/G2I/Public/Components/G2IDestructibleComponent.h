

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "G2IDestructibleComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDestructibleDestroyed);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2IDestructibleComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Destruction")
	FOnDestructibleDestroyed OnDestroy;

	UFUNCTION(BlueprintCallable, Category = "Destruction")
	void DestroyOwner();
};
