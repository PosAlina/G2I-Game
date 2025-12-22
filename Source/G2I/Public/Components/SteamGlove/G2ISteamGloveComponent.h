#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "G2ISteamGloveComponent.generated.h"

class UG2ISteamMovementComponent;
class UG2IReloadingComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class G2I_API UG2ISteamGloveComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gadgets)
	TObjectPtr<UG2IReloadingComponent> ReloadingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gadgets)
	TObjectPtr<UG2ISteamMovementComponent> SteamMovementComp;
	
public:
	
	UG2ISteamGloveComponent();

};
