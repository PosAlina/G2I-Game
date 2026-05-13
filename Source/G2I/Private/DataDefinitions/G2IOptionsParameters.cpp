#include "DataDefinitions/G2IOptionsParameters.h"
#include "G2I.h"
#include "GameFramework/GameUserSettings.h"

void UG2IOptionsParameters::SetupDefaultParameters()
{
	Settings = UGameUserSettings::GetGameUserSettings();
	if (!ensure(Settings))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Game User Settings"), *GetName());
	}
	
	MouseSensitive = DefaultMouseSensitive;
	bIsInvertedCameraHorizontalRotation = bIsDefaultInvertedCameraHorizontalRotation;
	bIsInvertedCameraVerticalRotation = bIsDefaultInvertedCameraVerticalRotation;
	
	SetScreenMode(DefaultScreenMode);
	SetScreenResolution(DefaultScreenResolution);
	SetFrameRate(DefaultFrameRate);
	SetVSync(bIsDefaultVSyncOn);
	//SetBrightness(DefaultBrightness);
	
	SetAntialiasing(DefaultAntialiasing);
	bCustomQuality = bDefaultCustomQuality;
	if (bCustomQuality)
	{
		SetTextureQuality(DefaultTextureQuality);
		SetShadowsQuality(DefaultShadowsQuality);
		SetEffectsQuality(DefaultEffectsQuality);
		SetPostProcessingQuality(DefaultPostProcessingQuality);
		SetAntialiasingQuality(DefaultShadowsQuality);
		SetGlobalIlluminationQuality(DefaultEffectsQuality);
		SetReflectionQuality(DefaultPostProcessingQuality);
	}
	else
	{
		SetOverallQuality(DefaultOverallQuality);
	}
}

void UG2IOptionsParameters::SetScreenMode(const EG2IScreenMode NewScreenMode)
{
	ScreenMode = NewScreenMode;
	
	if (!ensure(Settings))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Game User Settings"), *GetName());
		return;
	}
	switch (ScreenMode)
	{
	case EG2IScreenMode::Windowed:
		Settings->SetFullscreenMode(EWindowMode::Windowed);
		break;
	case EG2IScreenMode::WindowedFullscreen:
		Settings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
		break;
	case EG2IScreenMode::Fullscreen:
		Settings->SetFullscreenMode(EWindowMode::Fullscreen);
		break;
	default: ;
	}
	Settings->ApplySettings(true);
}

void UG2IOptionsParameters::SetScreenResolution(const EG2IScreenResolution NewScreenResolution)
{
	ScreenResolution = NewScreenResolution;

	if (!ensure(Settings))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Game User Settings"), *GetName());
		return;
	}
	switch (ScreenResolution)
	{
	case EG2IScreenResolution::Resolution1280On720:
		Settings->SetScreenResolution(FIntPoint(1280, 720));
		break;
	case EG2IScreenResolution::Resolution1920On1080:
		Settings->SetScreenResolution(FIntPoint(1920, 1080));
		break;
	case EG2IScreenResolution::Resolution2560On1440:
		Settings->SetScreenResolution(FIntPoint(2560, 1440));
		break;
	case EG2IScreenResolution::Resolution3840On2160:
		Settings->SetScreenResolution(FIntPoint(3840, 2160));
		break;
	default: ;
	}
	Settings->ApplySettings(true);
}

void UG2IOptionsParameters::SetFrameRate(const EG2IFrameRate NewFrameRate)
{
	FrameRate = NewFrameRate;
	
	if (!ensure(Settings))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Game User Settings"), *GetName());
		return;
	}
	switch (FrameRate)
	{
	case EG2IFrameRate::FPS30:
		Settings->SetFrameRateLimit(30.f);
		break;
	case EG2IFrameRate::FPS60:
		Settings->SetFrameRateLimit(60.f);
		break;
	case EG2IFrameRate::FPS120:
		Settings->SetFrameRateLimit(120.f);
		break;
	case EG2IFrameRate::FPSUnlimited:
		Settings->SetFrameRateLimit(0.f);
		break;
	default: ;
	}
	Settings->ApplySettings(true);
}

/*void UG2IOptionsParameters::SetBrightness(const float NewBrightness)
{
	Brightness = NewBrightness;
	if (APostProcessVolume *PostProcessVolume =
	 *Cast<APostProcessVolume>(UGameplayStatics::GetActorOfClass(GetWorld(), APostProcessVolume::StaticClass())))
	{
		PostProcessVolume->Settings.AutoExposureBias = Brightness;
	}
}*/

void UG2IOptionsParameters::SetVSync(const bool bIsNewVSync)
{
	bIsVSyncOn = bIsNewVSync;
	
	if (!ensure(Settings))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Game User Settings"), *GetName());
		return;
	}
	Settings->SetVSyncEnabled(bIsVSyncOn);
	Settings->ApplySettings(true);
}

void UG2IOptionsParameters::SetOverallQuality(const EG2IQuality NewQuality)
{
	bCustomQuality = false;
	OverallQuality = NewQuality;
	
	SetTextureQuality(OverallQuality);
	SetShadowsQuality(OverallQuality);
	SetEffectsQuality(OverallQuality);
	SetPostProcessingQuality(OverallQuality);
	SetAntialiasingQuality(OverallQuality);
	SetGlobalIlluminationQuality(OverallQuality);
	SetReflectionQuality(OverallQuality);
}

void UG2IOptionsParameters::SetTextureQuality(const EG2IQuality NewQuality)
{
	TextureQuality = NewQuality;

	if (!ensure(Settings))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Game User Settings"), *GetName());
	}
	const int32 QualityIndex = StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(NewQuality));
	Settings->SetTextureQuality(QualityIndex);
	Settings->ApplySettings(true);
}

void UG2IOptionsParameters::SetShadowsQuality(const EG2IQuality NewQuality)
{
	ShadowsQuality = NewQuality;
	
	if (!ensure(Settings))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Game User Settings"), *GetName());
	}
	const int32 QualityIndex = StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(NewQuality));
	Settings->SetShadowQuality(QualityIndex);
	Settings->ApplySettings(true);
}

void UG2IOptionsParameters::SetEffectsQuality(const EG2IQuality NewQuality)
{
	EffectsQuality = NewQuality;
	
	if (!ensure(Settings))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Game User Settings"), *GetName());
	}
	const int32 QualityIndex = StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(NewQuality));
	Settings->SetVisualEffectQuality(QualityIndex);
	Settings->ApplySettings(true);
}

void UG2IOptionsParameters::SetPostProcessingQuality(const EG2IQuality NewQuality)
{
	PostProcessingQuality = NewQuality;
	
	if (!ensure(Settings))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Game User Settings"), *GetName());
	}
	const int32 QualityIndex = StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(NewQuality));
	Settings->SetPostProcessingQuality(QualityIndex);
	Settings->ApplySettings(true);
}

void UG2IOptionsParameters::SetAntialiasingQuality(EG2IQuality NewQuality)
{
	AntialiasingQuality = NewQuality;
	
	if (!ensure(Settings))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Game User Settings"), *GetName());
	}
	const int32 QualityIndex = StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(NewQuality));
	Settings->SetAntiAliasingQuality(QualityIndex);
	Settings->ApplySettings(true);
}

void UG2IOptionsParameters::SetGlobalIlluminationQuality(EG2IQuality NewQuality)
{
	GlobalIlluminationQuality = NewQuality;
	
	if (!ensure(Settings))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Game User Settings"), *GetName());
	}
	const int32 QualityIndex = StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(NewQuality));
	Settings->SetGlobalIlluminationQuality(QualityIndex);
	Settings->ApplySettings(true);
}

void UG2IOptionsParameters::SetReflectionQuality(EG2IQuality NewQuality)
{
	ReflectionQuality = NewQuality;
	
	if (!ensure(Settings))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Game User Settings"), *GetName());
	}
	const int32 QualityIndex = StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(NewQuality));
	Settings->SetReflectionQuality(QualityIndex);
	Settings->ApplySettings(true);
}

void UG2IOptionsParameters::SetAntialiasing(const EG2IAntialiasing NewAntialiasing)
{
	Antialiasing = NewAntialiasing;
	
	IConsoleVariable *AntialiasingVariable =
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.AntiAliasingMethod"));

	if (!ensure(AntialiasingVariable))
	{
		UE_LOG(LogG2I, Error, TEXT("Console variable Antialiasing doesn't exist"));
		return;
	}
	
	switch (Antialiasing)
	{
	case EG2IAntialiasing::None:
		AntialiasingVariable->Set(TEXT("0"));
		break;
	case EG2IAntialiasing::FXAA:
		AntialiasingVariable->Set(TEXT("1"));
		break;
	case EG2IAntialiasing::TAA:
		AntialiasingVariable->Set(TEXT("2"));
		break;
	case EG2IAntialiasing::MSAA:
		AntialiasingVariable->Set(TEXT("3"));
		break;
	case EG2IAntialiasing::TSR:
		AntialiasingVariable->Set(TEXT("4"));
		break;
	}
}
