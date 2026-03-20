#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/EngineTypes.h"
#include "Components/AudioComponent.h"
#include "Components/SceneComponent.h"
#include "Sound/SoundCue.h"
#include "G2IGameSoundManager.generated.h"

USTRUCT(BlueprintType)
struct FSoundConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Sound")
	TObjectPtr<USoundCue> Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Location")
	FVector WorldLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Attachment")
	FComponentReference AttachToComponent;

	UPROPERTY()
	USceneComponent* ResolvedAttachComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Attachment")
	EAttachmentRule AttachmentRules = EAttachmentRule::SnapToTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Play", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Play", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float PitchMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Lifetime")
	bool bAutoDestroy = true;
};

UCLASS()
class G2I_API UG2IGameSoundManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
private:

	UPROPERTY()
	TArray<int32> IdStack;

	int32 CurrentNumberAvailable;

	UPROPERTY()
	TMap<int32, TObjectPtr<UAudioComponent>> ActiveSounds;

	void UpdateStackSize();

protected:
	
	UAudioComponent* GetAudioById(int32 SoundId);
public:

	UFUNCTION(BlueprintPure, Category = "Sound Manager", meta = (WorldContext = "WorldContextObject"))
	static UG2IGameSoundManager* Get(UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	int32 AddSound(const FSoundConfig& NewSoundConfig);
	bool RemoveSound(int32 SoundId);
	void RemoveAllSounds();

	bool PlaySound(int32 SoundId);
	bool StopSound(int32 SoundId);
	void StopAllSounds();

	bool ChangeSoundVolume(int32 SoundId, float NewVolume);
	bool ChangeSoundPitch(int32 SoundId, float NewPitch);
	bool ChangeSoundLocation(int32 SoundId, FVector NewLocation);
	bool ChangeSoundAttachment(int32 SoundId,
		USceneComponent* NewAttachementComponent,
		EAttachmentRule AttachmentRules = EAttachmentRule::SnapToTarget);
	bool ChangeSoundAttachment(int32 SoundId,
		AActor* NewAttachementActor,
		EAttachmentRule AttachmentRules = EAttachmentRule::SnapToTarget);
};
