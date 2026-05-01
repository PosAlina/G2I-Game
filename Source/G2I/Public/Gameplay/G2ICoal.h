#pragma once

#include "CoreMinimal.h"
#include "Interfaces/G2IActivationInterface.h"
#include "GameFramework/Actor.h"
#include "G2ICoal.generated.h"

class UG2IDestructibleComponent;
class UG2ISoundComponent;

UCLASS(Blueprintable, Placeable)
class G2I_API AG2ICoal : public AActor, public IG2IActivationInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRootComponent;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


	int32 HitDestroySoundId;
	int32 VoiceHitSoundId;
	int32 FurnaceDestroySoundId;
	int32 VoiceFurnaceSoundId;

public:
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<UG2ISoundComponent> SoundComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName GrabberNameTag = FName("Grabber");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName VoiceFurnaceSoundName = FName("VoiceFurnaceSound");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName VoiceHitSoundName = FName("VoiceHitSound");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName HitDestroySoundName = FName("HitDestroySound");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FurnaceDestroySoundName = FName("FurnaceDestroySound");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CoalSpawnLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator CoalSpawnRotation;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|Components")
	TObjectPtr<UG2IDestructibleComponent> DestructComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	virtual void Activate_Implementation() override;
	virtual void Deactivate_Implementation() override;

	AG2ICoal();
};
