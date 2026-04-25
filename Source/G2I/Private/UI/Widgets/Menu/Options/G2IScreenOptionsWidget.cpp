#include "Menu/Options/G2IScreenOptionsWidget.h"
#include "G2I.h"
#include "G2IUIManager.h"
#include "Menu/Elements/TextRow/G2ITextPropertySelector.h"

void UG2IScreenOptionsWidget::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	InitializeDefaults();
}

void UG2IScreenOptionsWidget::InitializeDefaults() const
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	
	if (ensure(ScreenMode))
	{
		TArray<FString> PropertiesValue;
		PropertiesValue.Add("Screen.Property.ScreenMode.Value.Windowed");
		PropertiesValue.Add("Screen.Property.ScreenMode.Value.WindowedFullscreen");
		PropertiesValue.Add("Screen.Property.ScreenMode.Value.Fullscreen");
		UIManager->SetPropertyRow(ScreenMode, "Screen.Property.ScreenMode",
			PropertiesValue, 2);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Screen Mode\""), *GetName());
	}
	if (ensure(ScreenResolution))
	{
		TArray<FString> PropertiesValue;
		PropertiesValue.Add("Screen.Property.ScreenResolution.Value.1280x720");
		PropertiesValue.Add("Screen.Property.ScreenResolution.Value.1920x1080");
		PropertiesValue.Add("Screen.Property.ScreenResolution.Value.2560x1440");
		PropertiesValue.Add("Screen.Property.ScreenResolution.Value.3840x2160");
		UIManager->SetPropertyRow(ScreenResolution, "Screen.Property.ScreenResolution",
			PropertiesValue, 1);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Screen resolution\""), *GetName());
	}
	if (ensure(FrameRate))
	{
		TArray<FString> PropertiesValue;
		PropertiesValue.Add("Screen.Property.FrameRate.Value.30FPS");
		PropertiesValue.Add("Screen.Property.FrameRate.Value.60FPS");
		PropertiesValue.Add("Screen.Property.FrameRate.Value.120FPS");
		PropertiesValue.Add("Screen.Property.FrameRate.Value.Unlimited");
		UIManager->SetPropertyRow(FrameRate, "Screen.Property.FrameRate",
			PropertiesValue, 3);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Frame rate\""), *GetName());
	}
	if (ensure(Brightness))
	{
		UIManager->SetPropertyRow(Brightness, "Screen.Property.Brightness",
			0, 10, 1, 10, 0);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Frame rate\""), *GetName());
	}
	if (ensure(VSync))
	{
		TArray<FString> PropertiesValue;
		PropertiesValue.Add("Property.Value.Off");
		PropertiesValue.Add("Property.Value.On");
		UIManager->SetPropertyRow(VSync, "Screen.Property.VSync",
			PropertiesValue, 1);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"VSync\""), *GetName());
	}
}

void UG2IScreenOptionsWidget::ApplyOptions_Implementation()
{
	Super::ApplyOptions_Implementation();
	
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->ApplyPropertiesValues({ScreenMode, ScreenResolution, FrameRate, VSync});
}

void UG2IScreenOptionsWidget::CancelUnAppliedOptions_Implementation()
{
	Super::CancelUnAppliedOptions_Implementation();
	
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->CancelUnAppliedPropertiesValues({ScreenMode, ScreenResolution, FrameRate, VSync});
}
