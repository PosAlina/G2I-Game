#include "Game/G2ISavingTriggerBox.h"
#include "Interfaces/SavingSystem/G2ISaveGameplayInterface.h"
#include "G2I.h"

AG2ISavingTriggerBox::AG2ISavingTriggerBox()
{
	OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);

	FGameplayTag SavableTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Savable"));
	if (SavableTag.IsValid())
	{
		GameplayTags.AddTag(SavableTag);
	}

#if WITH_EDITOR
	SetActorHiddenInGame(false);
#endif
}

void AG2ISavingTriggerBox::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = GetGameInstance();
	if (GameInstance && GameInstance->Implements<UG2ISaveGameplayInterface>())
	{
		IG2ISaveGameplayInterface::Execute_LoadRequestedData(GameInstance, this);
		if (auto* Delegates = IG2ISaveGameplayInterface::Execute_GetGameplaySaveDelegates(GameInstance))
			Delegates->OnGameplaySavedDelegate.AddUniqueDynamic(this, &ThisClass::OnGameplaySaved);
	}
	else
		UE_LOG(LogG2I, Warning, TEXT("%s doesn't implement interface UG2ISaveGameplayInterface or is NULL in %s."), *GameInstance->GetName(), *GetName());
}

void AG2ISavingTriggerBox::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	bActivated = true;

	if (GameInstance && GameInstance->Implements<UG2ISaveGameplayInterface>())
	{
		IG2ISaveGameplayInterface::Execute_SaveAllDataAndGameplay(GameInstance, true);
	}
	else
		UE_LOG(LogG2I, Warning, TEXT("%s doesn't implement interface UG2ISaveGameplayInterface or is NULL in %s."), *GameInstance->GetName(), *GetName());
}

void AG2ISavingTriggerBox::OnGameplaySaved(bool bSuccess)
{
	if (bActivated)
		Destroy();
}

void AG2ISavingTriggerBox::SaveData_Implementation(UG2IGameplaySaveGame* SaveGameRef)
{
	if (SaveGameRef)
	{
		SaveGameRef->SaveTriggerBoxesSaveData.Add(GetName(), bActivated);
	}
}

void AG2ISavingTriggerBox::LoadData_Implementation(const UG2IGameplaySaveGame* SaveGameRef)
{
	if (SaveGameRef)
	{
		if (auto* Key = SaveGameRef->SaveTriggerBoxesSaveData.Find(GetName()))
			if (&Key)
				Destroy();
	}
}
