#pragma once

#include "CoreMinimal.h"
#include "G2IMechanicUsingAimInterface.h"
#include "Components/ActorComponent.h"
#include "G2ISteamShotInputInterface.h"
#include "G2ISteamShotComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FComponentActivateDelegate, float, ShotDistance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAimTypeChangedDelegate, EG2IAimType, NewAimType);

UCLASS(ClassGroup=(SteamGlove), meta=(BlueprintSpawnableComponent))
class G2I_API UG2ISteamShotComponent : public USceneComponent, public IG2ISteamShotInputInterface,
	public IG2IMechanicUsingAimInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FComponentActivateDelegate OnComponentActivateDelegate;

	UPROPERTY(BlueprintAssignable)
	FAimTypeChangedDelegate OnAimTypeAfterHitChangedDelegate;
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ShotDetails)
	float ShotDistance = 2000.f;

private:

	UPROPERTY()
	TObjectPtr<UWorld> World;
	
public:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = Input)
	virtual void ShootAction_Implementation(const FG2IHitInfo TargetHitInfo) override;

	UFUNCTION(BlueprintCallable, Category = Aiming)
	virtual void ActivateForAim_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = Aiming)
	virtual EG2IAimType GetAimTypeByActor_Implementation(const AActor* TargetActor) override;
	
};
