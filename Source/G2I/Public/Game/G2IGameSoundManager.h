#pragma once



#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "G2IGameSoundManager.generated.h"

class UAudioComponent;

UCLASS()
class G2I_API AG2IGameSoundManager : public AGameStateBase
{
	GENERATED_BODY()
	
private:
	TArray<int32> IdStack;
	UPROPERTY()
	TMap<int32, UAudioComponent*> ActiveSounds;

protected:
	virtual void BeginPlay() override;
public:

	int AddSound(USoundCue* NewSound);
	bool RemoveSound(int SoundId);
};
