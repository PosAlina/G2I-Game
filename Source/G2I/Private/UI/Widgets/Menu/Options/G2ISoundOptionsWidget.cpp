#include "Menu/Options/G2ISoundOptionsWidget.h"
#include "Menu/Elements/NumericalRow/G2INumericalMultiValuePropertyRow.h"
#include "Menu/Elements/TextRow/G2ITextMultiValuePropertyRow.h"
#include "G2I.h"
#include "G2IOptionsParameters.h"
#include "G2IUIManager.h"
#include "G2IGameInstance.h"
#include "G2IGameSoundManager.h"

void UG2ISoundOptionsWidget::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();
	
	InitializeSoundManager();
	InitializeVolumesData();
	InitializeDefaults();
}

void UG2ISoundOptionsWidget::InitializeDefaults() const
{
	InitializeCommonVolume();
	InitializeMusicVolume();
	InitializeEffectsVolume();
	InitializeDialoguesVolume();
}

void UG2ISoundOptionsWidget::ApplyOptions_Implementation()
{
	Super::ApplyOptions_Implementation();
	
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->ApplyPropertiesValues(
		{CommonVolume, MusicVolume, EffectsVolume, DialoguesVolume});
}

void UG2ISoundOptionsWidget::CancelUnAppliedOptions_Implementation()
{
	Super::CancelUnAppliedOptions_Implementation();
	
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->CancelUnAppliedPropertiesValues(
		{CommonVolume, MusicVolume, EffectsVolume, DialoguesVolume});
}


void UG2ISoundOptionsWidget::InitializeCommonVolume() const 
{
	if (!ensure(CommonVolume))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"CommonVolume\""), *GetName());
		return;
	}
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	if (!ensure(OptionsParameters))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
			*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
		return;
	}

	const float DefaultCommonVolume = FMath::Clamp(OptionsParameters->DefaultCommonVolume, MinVolume, MaxVolume);
	OptionsParameters->CommonVolume = DefaultCommonVolume;

	UIManager->SetPropertyRow(CommonVolume, "Sounds.Property.CommonVolume",
			MinVolume, MaxVolume, StepVolume, DefaultCommonVolume, 0);

	CommonVolume->OnApplyPropertyValue = [this]()
		{
			if (!ensure(OptionsParameters))
			{
				UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
					*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
				return;
			}
			if (!ensure(SoundManager))
			{
				UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
					*GetName(), *UG2IGameSoundManager::StaticClass()->GetName());
				return;
			}
			SoundManager->SetGlobalVolume(EG2ISoundType::DefaultSound, CommonVolume->GetCurrentValue());
		};

	CommonVolume->OnCancelUnAppliedPropertyValue = [this]()
		{
			if (!ensure(OptionsParameters))
			{
				UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
					*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
				return;
			}
			const float OldCommonVolume = OptionsParameters->CommonVolume;
			CommonVolume->SelectValue(OldCommonVolume);
		};
}

void UG2ISoundOptionsWidget::InitializeMusicVolume() const {
	if (!ensure(MusicVolume))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"MusicVolume\""), *GetName());
		return;
	}
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	if (!ensure(OptionsParameters))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
			*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
		return;
	}

	const float DefaultMusicVolume = FMath::Clamp(OptionsParameters->DefaultMusicVolume, MinVolume, MaxVolume);
	OptionsParameters->MusicVolume = DefaultMusicVolume;

	UIManager->SetPropertyRow(MusicVolume, "Sounds.Property.MusicVolume",
		MinVolume, MaxVolume, StepVolume, DefaultMusicVolume, 0);

	MusicVolume->OnApplyPropertyValue = [this]()
		{
			if (!ensure(OptionsParameters))
			{
				UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
					*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
				return;
			}
			if (!ensure(SoundManager))
			{
				UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
					*GetName(), *UG2IGameSoundManager::StaticClass()->GetName());
				return;
			}
			SoundManager->SetGlobalVolume(EG2ISoundType::MusicSound, MusicVolume->GetCurrentValue());
		};

	MusicVolume->OnCancelUnAppliedPropertyValue = [this]()
		{
			if (!ensure(OptionsParameters))
			{
				UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
					*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
				return;
			}
			const float OldMusicVolume = OptionsParameters->MusicVolume;
			MusicVolume->SelectValue(OldMusicVolume);
		};
}

void UG2ISoundOptionsWidget::InitializeEffectsVolume() const {
	if (!ensure(EffectsVolume))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"EffectsVolume\""), *GetName());
		return;
	}
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	if (!ensure(OptionsParameters))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
			*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
		return;
	}

	const float DefaultEffectsVolume = FMath::Clamp(OptionsParameters->DefaultEffectsVolume, MinVolume, MaxVolume);
	OptionsParameters->EffectsVolume = DefaultEffectsVolume;

	UIManager->SetPropertyRow(EffectsVolume, "Sounds.Property.EffectsVolume",
		MinVolume, MaxVolume, StepVolume, DefaultEffectsVolume, 0);

	EffectsVolume->OnApplyPropertyValue = [this]()
		{
			if (!ensure(OptionsParameters))
			{
				UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
					*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
				return;
			}
			if (!ensure(SoundManager))
			{
				UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
					*GetName(), *UG2IGameSoundManager::StaticClass()->GetName());
				return;
			}
			SoundManager->SetGlobalVolume(EG2ISoundType::EffectSound, EffectsVolume->GetCurrentValue());
		};

	EffectsVolume->OnCancelUnAppliedPropertyValue = [this]()
		{
			if (!ensure(OptionsParameters))
			{
				UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
					*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
				return;
			}
			const float OldEffectsVolume = OptionsParameters->EffectsVolume;
			EffectsVolume->SelectValue(OldEffectsVolume);
		};
}

void UG2ISoundOptionsWidget::InitializeDialoguesVolume() const {
	if (!ensure(DialoguesVolume))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"DialoguesVolume\""), *GetName());
		return;
	}
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	if (!ensure(OptionsParameters))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
			*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
		return;
	}

	const float DefaultDialoguesVolume = FMath::Clamp(OptionsParameters->DefaultDialoguesVolume, MinVolume, MaxVolume);
	OptionsParameters->DialoguesVolume = DefaultDialoguesVolume;

	UIManager->SetPropertyRow(DialoguesVolume, "Sounds.Property.DialoguesVolume",
		MinVolume, MaxVolume, StepVolume, DefaultDialoguesVolume, 0);

	DialoguesVolume->OnApplyPropertyValue = [this]()
		{
			if (!ensure(OptionsParameters))
			{
				UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
					*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
				return;
			}
			if (!ensure(SoundManager))
			{
				UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
					*GetName(), *UG2IGameSoundManager::StaticClass()->GetName());
				return;
			}
			SoundManager->SetGlobalVolume(EG2ISoundType::SpeechSound, DialoguesVolume->GetCurrentValue());
		};

	DialoguesVolume->OnCancelUnAppliedPropertyValue = [this]()
		{
			if (!ensure(OptionsParameters))
			{
				UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
					*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
				return;
			}
			const float OldCommonVolume = OptionsParameters->DialoguesVolume;
			DialoguesVolume->SelectValue(OldCommonVolume);
		};
}

void UG2ISoundOptionsWidget::InitializeSoundManager(){
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("Game Instance doesn't exist in %s"), *GetName());
		return;
	}

	SoundManager = GameInstance->GetSubsystem<UG2IGameSoundManager>();
	if (!ensure(SoundManager))
	{
		UE_LOG(LogG2I, Error, TEXT("Couldn't get %s subsystem from GameInstance in %s"), *UG2IGameSoundManager::StaticClass()->GetName(),
			*GetName());
	}
}

void UG2ISoundOptionsWidget::InitializeVolumesData() {
	if (!ensure(OptionsParameters))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
			*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
		return;
	}

	MinVolume = OptionsParameters->MinVolume;
	MaxVolume = OptionsParameters->MaxVolume;
	StepVolume = OptionsParameters->StepVolume;
}