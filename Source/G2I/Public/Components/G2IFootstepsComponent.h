#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "G2IFootstepsComponent.generated.h"

class UG2ISoundComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2IFootstepsComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	TMap<FName, int32> SoundIDs;

	UPROPERTY()
	TObjectPtr<UG2ISoundComponent> SoundComp;

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<EPhysicalSurface>, FName> SoundsForSurfaces;

public:
	UG2IFootstepsComponent();

	void PlaySoundBasedOnPhysSurface(const EPhysicalSurface& SurfaceType);

protected:
	virtual void BeginPlay() override;
};
