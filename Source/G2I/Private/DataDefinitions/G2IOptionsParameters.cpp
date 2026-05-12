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