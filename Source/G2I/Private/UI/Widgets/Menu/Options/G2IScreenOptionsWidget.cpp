#include "Menu/Options/G2IScreenOptionsWidget.h"
#include "G2I.h"
#include "G2IOptionsParameters.h"
#include "G2IUIManager.h"
#include "Menu/Elements/TextRow/G2ITextPropertySelector.h"

void UG2IScreenOptionsWidget::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	InitializeDefaults();
}

void UG2IScreenOptionsWidget::InitializeDefaults() const
{
	InitializeScreenMode();
	InitializeScreenResolution();
	InitializeFrameRate();
	//InitializeBrightness();
	InitializeVSync();
}

void UG2IScreenOptionsWidget::InitializeScreenMode() const
{
	if (!ensure(ScreenMode))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Screen Mode\""), *GetName());
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
	
	TArray<FString> PropertiesValue;
	PropertiesValue.Add("Screen.Property.ScreenMode.Value.Windowed");
	PropertiesValue.Add("Screen.Property.ScreenMode.Value.WindowedFullscreen");
	PropertiesValue.Add("Screen.Property.ScreenMode.Value.Fullscreen");

	const int32 DefaultValue =
		StaticEnum<EG2IScreenMode>()->GetIndexByValue(static_cast<int32>(OptionsParameters->ScreenMode));
	
	
	UIManager->SetPropertyRow(ScreenMode, "Screen.Property.ScreenMode",
			PropertiesValue, DefaultValue);
	
	ScreenMode->OnApplyPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}

		switch (ScreenMode->GetCurrentValueIndex())
		{
		case 0:
			OptionsParameters->SetScreenMode(EG2IScreenMode::Windowed);
			break;
		case 1:
			OptionsParameters->SetScreenMode(EG2IScreenMode::WindowedFullscreen);
			break;
		case 2:
			OptionsParameters->SetScreenMode(EG2IScreenMode::Fullscreen);
			break;
		default: ;
		}
	};
	
	ScreenMode->OnCancelUnAppliedPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		const int32 OldValue =
			StaticEnum<EG2IScreenMode>()->GetIndexByValue(static_cast<int32>(OptionsParameters->ScreenMode));
		ScreenMode->SelectValueByIndex(OldValue);
	};
}

void UG2IScreenOptionsWidget::InitializeScreenResolution() const
{
	if (!ensure(ScreenResolution))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Screen resolution\""), *GetName());
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
	
	TArray<FString> PropertiesValue;
	PropertiesValue.Add("Screen.Property.ScreenResolution.Value.1280x720");
	PropertiesValue.Add("Screen.Property.ScreenResolution.Value.1920x1080");
	PropertiesValue.Add("Screen.Property.ScreenResolution.Value.2560x1440");
	PropertiesValue.Add("Screen.Property.ScreenResolution.Value.3840x2160");

	const int32 DefaultValue =
		StaticEnum<EG2IFrameRate>()->GetIndexByValue(static_cast<int32>(OptionsParameters->ScreenResolution));
	
	UIManager->SetPropertyRow(ScreenResolution, "Screen.Property.ScreenResolution",
			PropertiesValue, DefaultValue);
	
	ScreenResolution->OnApplyPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}

		switch (ScreenResolution->GetCurrentValueIndex())
		{
		case 0:
			OptionsParameters->SetScreenResolution(EG2IScreenResolution::Resolution1280On720);
			break;
		case 1:
			OptionsParameters->SetScreenResolution(EG2IScreenResolution::Resolution1920On1080);
			break;
		case 2:
			OptionsParameters->SetScreenResolution(EG2IScreenResolution::Resolution2560On1440);
			break;
		case 3:
			OptionsParameters->SetScreenResolution(EG2IScreenResolution::Resolution3840On2160);
			break;
		default: ;
		}
	};
	
	ScreenResolution->OnCancelUnAppliedPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		const int32 OldValue =
			StaticEnum<EG2IScreenResolution>()->GetIndexByValue(static_cast<int32>(OptionsParameters->ScreenResolution));
		ScreenResolution->SelectValueByIndex(OldValue);
	};
}

void UG2IScreenOptionsWidget::InitializeFrameRate() const
{
	if (!ensure(FrameRate))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Frame rate\""), *GetName());
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
	
	TArray<FString> PropertiesValue;
	PropertiesValue.Add("Screen.Property.FrameRate.Value.30FPS");
	PropertiesValue.Add("Screen.Property.FrameRate.Value.60FPS");
	PropertiesValue.Add("Screen.Property.FrameRate.Value.120FPS");
	PropertiesValue.Add("Screen.Property.FrameRate.Value.Unlimited");

	const int32 DefaultValue =
		StaticEnum<EG2IFrameRate>()->GetIndexByValue(static_cast<int32>(OptionsParameters->FrameRate));
	
	UIManager->SetPropertyRow(FrameRate, "Screen.Property.FrameRate",
		PropertiesValue, DefaultValue);

	FrameRate->OnApplyPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		
		switch (FrameRate->GetCurrentValueIndex())
		{
		case 0:
			OptionsParameters->SetFrameRate(EG2IFrameRate::FPS30);
			break;
		case 1:
			OptionsParameters->SetFrameRate(EG2IFrameRate::FPS60);
			break;
		case 2:
			OptionsParameters->SetFrameRate(EG2IFrameRate::FPS120);
			break;
		case 3:
			OptionsParameters->SetFrameRate(EG2IFrameRate::FPSUnlimited);
			break;
		default: ;
		}
	};
	
	FrameRate->OnCancelUnAppliedPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		const int32 OldValue =
			StaticEnum<EG2IFrameRate>()->GetIndexByValue(static_cast<int32>(OptionsParameters->FrameRate));
		FrameRate->SelectValueByIndex(OldValue);
	};
}

/*void UG2IScreenOptionsWidget::InitializeBrightness() const
{
	if (!ensure(Brightness))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Brightness\""), *GetName());
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
	
	const float MinBrightness = FMath::Max(OptionsParameters->MinBrightness, 0.f);
	OptionsParameters->MinBrightness = MinBrightness;
	
	const float MaxBrightness =
		FMath::Max(OptionsParameters->MaxBrightness, OptionsParameters->MinBrightness);
	OptionsParameters->MaxBrightness = MaxBrightness;
	
	const float CurrentBrightness =
		FMath::Clamp(OptionsParameters->Brightness, MinBrightness, MaxBrightness);
	OptionsParameters->DefaultBrightness = CurrentBrightness;
	
	const float StepBrightness =
		FMath::Clamp(OptionsParameters->StepBrightness, MinBrightness, MaxBrightness);
	OptionsParameters->StepBrightness = StepBrightness;

	UIManager->SetPropertyRow(Brightness, "Screen.Property.Brightness",
			MinBrightness, MaxBrightness, StepBrightness, CurrentBrightness, 0);
	
	Brightness->OnApplyPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		OptionsParameters->SetBrightness(Brightness->GetCurrentValue());
	};
	
	Brightness->OnCancelUnAppliedPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		const float OldBrightness = OptionsParameters->Brightness;
		Brightness->SelectValue(OldBrightness);
	};
}*/

void UG2IScreenOptionsWidget::InitializeVSync() const
{
	if (!ensure(VSync))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"VSync\""), *GetName());
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
	
	TArray<FString> PropertiesValue;
	PropertiesValue.Add("Property.Value.Off");
	PropertiesValue.Add("Property.Value.On");
	
	const int32 DefaultValue = OptionsParameters->bIsDefaultVSyncOn ? 1 : 0;
	
	UIManager->SetPropertyRow(VSync, "Screen.Property.VSync",
		PropertiesValue, DefaultValue);
	
	VSync->OnApplyPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		OptionsParameters->SetVSync(VSync->GetCurrentValueIndex() != 0.f);
	};
	
	VSync->OnCancelUnAppliedPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		const int32 OldValue = OptionsParameters->bIsVSyncOn ? 1 : 0;
		VSync->SelectValueByIndex(OldValue);
	};
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
