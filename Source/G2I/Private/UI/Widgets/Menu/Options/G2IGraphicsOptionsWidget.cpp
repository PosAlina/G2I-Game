#include "Menu/Options/G2IGraphicsOptionsWidget.h"
#include "G2I.h"
#include "G2IUIManager.h"
#include "Menu/Elements/TextRow/G2ITextMultiValuePropertyRow.h"

void UG2IGraphicsOptionsWidget::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	InitializeDefaults();
	BindDelegates();
}

void UG2IGraphicsOptionsWidget::InitializeDefaults()
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}

	TArray<FString> PropertiesQualityValue;
	PropertiesQualityValue.Add("Graphics.Property.Quality.Value.Low"); //Index of quality: 0
	PropertiesQualityValue.Add("Graphics.Property.Quality.Value.Medium"); //Index of quality: 1
	PropertiesQualityValue.Add("Graphics.Property.Quality.Value.High"); //Index of quality: 2
	PropertiesQualityValue.Add("Graphics.Property.Quality.Value.Ultra"); //Index of quality: 3
	PropertiesQualityValue.Add("Graphics.Property.Quality.Value.Epic"); //Index of quality: 4

	if (ensure(OverallQuality))
	{
		TArray<FString> OverallPropertiesQualityValue = PropertiesQualityValue;
		OverallPropertiesQualityValue.Add("Graphics.Property.Quality.Value.Custom"); //Index of quality: 5
		UIManager->SetPropertyRow(OverallQuality, "Graphics.Property.OverallQuality",
			OverallPropertiesQualityValue, DefaultQualityIndex);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Overall Quality\""), *GetName());
	}
	if (ensure(TextureQuality))
	{
		UIManager->SetPropertyRow(TextureQuality, "Graphics.Property.TextureQuality",
			PropertiesQualityValue, DefaultQualityIndex);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Texture Quality\""), *GetName());
	}
	if (ensure(ShadowsQuality))
	{
		UIManager->SetPropertyRow(ShadowsQuality, "Graphics.Property.ShadowsQuality",
			PropertiesQualityValue, DefaultQualityIndex);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Shadows Quality\""), *GetName());
	}
	if (ensure(EffectsQuality))
	{
		UIManager->SetPropertyRow(EffectsQuality, "Graphics.Property.EffectsQuality",
			PropertiesQualityValue, DefaultQualityIndex);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Texture Quality\""), *GetName());
	}
	if (ensure(PostProcessingQuality))
	{
		UIManager->SetPropertyRow(PostProcessingQuality, "Graphics.Property.PostProcessingQuality",
			PropertiesQualityValue, DefaultQualityIndex);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Post Processing Quality\""), *GetName());
	}
	if (ensure(Antialiasing))
	{
		TArray<FString> PropertiesValue;
		PropertiesValue.Add("Graphics.Property.Antialiasing.Value.None");//Low quality of graphic
		PropertiesValue.Add("Graphics.Property.Antialiasing.Value.FXAA");//Medium quality of graphic
		PropertiesValue.Add("Graphics.Property.Antialiasing.Value.MSAA");//High quality of graphic
		PropertiesValue.Add("Graphics.Property.Antialiasing.Value.TAA"); //Epic quality of graphic
		PropertiesValue.Add("Graphics.Property.Antialiasing.Value.TSR");//Ultra quality of graphic
		UIManager->SetPropertyRow(Antialiasing, "Graphics.Property.Antialiasing",
			PropertiesValue, DefaultQualityIndex);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Antialiasing\""), *GetName());
	}

	if (OverallQuality)
	{
		SetPropertiesByQuality(OverallQuality->GetCurrentValueIndex());
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Overall Quality\""), *GetName());
	}
}

void UG2IGraphicsOptionsWidget::BindDelegates()
{
	if (ensure(OverallQuality))
	{
		OverallQuality->OnSelectValueDelegate.AddDynamic(this, &ThisClass::SetPropertiesByQuality);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Overall Quality\""), *GetName());
	}
}

void UG2IGraphicsOptionsWidget::ApplyPropertiesValues() const
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->ApplyPropertiesValues({
		OverallQuality, TextureQuality, ShadowsQuality, PostProcessingQuality, Antialiasing});
}

void UG2IGraphicsOptionsWidget::SavePropertiesValues() const
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->SavePropertiesValues(
		{OverallQuality, TextureQuality, ShadowsQuality, PostProcessingQuality, Antialiasing});
}

void UG2IGraphicsOptionsWidget::SetPropertiesByQuality(const int32 Quality)
{
	SetPropertyByQuality(TextureQuality, Quality);
	SetPropertyByQuality(ShadowsQuality, Quality);
	SetPropertyByQuality(EffectsQuality, Quality);
	SetPropertyByQuality(PostProcessingQuality, Quality);
	SetPropertyByQuality(Antialiasing, Quality);
}

void UG2IGraphicsOptionsWidget::SetPropertyByQuality(UG2ITextMultiValuePropertyRow* Property, const int32 Quality) const
{
	if (!ensure(Property))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property"), *GetName());
		return;
	}
	
	if (Quality == CustomQualityIndex)
	{
		Property->SetIsEnabled(true);
	}
	else
	{
		Property->SetIsEnabled(false);
		Property->SelectValueByIndex(Quality);
	}
}
