#include "Menu/Options/G2ISoundOptionsWidget.h"
#include "Menu/Elements/NumericalRow/G2INumericalMultiValuePropertyRow.h"
#include "Menu/Elements/TextRow/G2ITextMultiValuePropertyRow.h"
#include "G2I.h"
#include "G2IUIManager.h"

void UG2ISoundOptionsWidget::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	InitializeDefaults();
}

void UG2ISoundOptionsWidget::InitializeDefaults() const
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	
	if (ensure(CommonVolume))
	{
		
		UIManager->SetPropertyRow(CommonVolume, "Sounds.Property.CommonVolume",
			0, 10, 1, DefaultVolumeIndex, 0);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Common Volume\""), *GetName());
	}
	if (ensure(MusicVolume))
	{
		UIManager->SetPropertyRow(MusicVolume, "Sounds.Property.MusicVolume",
			0, 10, 1, DefaultVolumeIndex, 0);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Music Volume\""), *GetName());
	}
	if (ensure(EffectsVolume))
	{
		UIManager->SetPropertyRow(EffectsVolume, "Sounds.Property.EffectsVolume",
			0, 10, 1, DefaultVolumeIndex, 0);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Effects Volume\""), *GetName());
	}
	if (ensure(DialoguesVolume))
	{
		UIManager->SetPropertyRow(DialoguesVolume, "Sounds.Property.DialoguesVolume",
			0, 10, 1, DefaultVolumeIndex, 0);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Dialogues Volume\""), *GetName());
	}
	if (ensure(Subtitles))
	{
		TArray<FString> PropertiesValue;
		PropertiesValue.Add("Property.Value.Off");
		PropertiesValue.Add("Property.Value.On");
		UIManager->SetPropertyRow(Subtitles, "Sounds.Property.Subtitles",
			PropertiesValue, 1);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Subtitles\""), *GetName());
	}
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
		{CommonVolume, MusicVolume, EffectsVolume, DialoguesVolume, Subtitles});
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
		{CommonVolume, MusicVolume, EffectsVolume, DialoguesVolume, Subtitles});
}
