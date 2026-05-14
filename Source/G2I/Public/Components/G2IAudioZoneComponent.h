#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "G2IAudioZoneComponent.generated.h"

class UG2ISoundComponent;
class UG2ILauncherComponent;
class UPrimitiveComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class G2I_API UG2IAudioZoneComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	int32 PlayingSoundId = -1;

	UPROPERTY(Transient)
	TObjectPtr<UPrimitiveComponent> AudioZoneTriggerCollision;

public:
	UG2IAudioZoneComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Audio_Zone")
	TObjectPtr<UG2ISoundComponent> SoundComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Audio_Zone")
	TObjectPtr<UG2ILauncherComponent> LauncherComp;

	UPROPERTY(EditAnywhere, Category = "Data|Audio_Zone")
	FComponentReference AudioZoneTriggerCollisionRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Audio_Zone")
	TSet<TObjectPtr<AActor>> AllowedActors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Audio_Zone")
	bool bIsStopWhenLeaving = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Audio_Zone")
	FName PlayingSoundName = FName("ZonePlayingSound");

	virtual void BeginPlay() override;
protected:

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};