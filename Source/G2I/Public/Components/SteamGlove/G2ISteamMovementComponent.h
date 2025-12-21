#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SteamGlove/G2ISteamMovementInputInterface.h"
#include "G2ISteamMovementComponent.generated.h"

class UG2ICharacterMovementComponent;
class UG2IReloadingComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartSteamJumpDelegate);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class G2I_API UG2ISteamMovementComponent : public UActorComponent, public IG2ISteamMovementInputInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FStartSteamJumpDelegate OnStartSteamJumpDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Jump)
	float SteamJumpImpulseStrength = 750.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Jump)
	int32 SteamJumpMaxCount = 1;

protected:
	
	int32 SteamJumpCurrentCount = 0;

	bool bCanSteamJump = true;

private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UActorComponent> MovementComponent;

public:

	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void SteamJumpAction_Implementation() override;

protected:

	UFUNCTION()
	void HandleLanded(const FHitResult& Hit);
	
	UFUNCTION()
	void EnableSteamJump();

	UFUNCTION()
	void DisableSteamJump();

private:

	void SetupDefaults();

	void BindDelegates();
	
};
