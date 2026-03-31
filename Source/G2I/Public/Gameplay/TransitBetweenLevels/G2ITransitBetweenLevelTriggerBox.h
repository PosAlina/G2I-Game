#pragma once

#include "CoreMinimal.h"
#include "G2ILockingInterface.h"
#include "Engine/TriggerBox.h"
#include "G2ITransitBetweenLevelTriggerBox.generated.h"

class UG2ILauncherComponent;
class UG2ITransitBetweenLevelsComponent;

UCLASS()
class G2I_API AG2ITransitBetweenLevelTriggerBox : public ATriggerBox, public IG2ILockingInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLocked = false;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2ITransitBetweenLevelsComponent> TransitComp;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2ILauncherComponent> LauncherComp;

public:

	AG2ITransitBetweenLevelTriggerBox();
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void SetIsLocked_Implementation(bool bIsNewLocked) override;
	virtual bool IsLocked_Implementation() override;

protected:

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void Locked(UG2ILauncherComponent* LauncherComponent, AActor* ComponentOwner, bool bIsNewLocked);
};
