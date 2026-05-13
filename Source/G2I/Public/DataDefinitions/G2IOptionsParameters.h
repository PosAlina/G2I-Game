#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "G2IOptionsParameters.generated.h"

UENUM(BlueprintType)
enum class EG2IScreenMode : uint8
{
	Windowed UMETA(DisplayName = "Windowed"),
	WindowedFullscreen UMETA(DisplayName = "Windowed fullscreen"),
	Fullscreen UMETA(DisplayName = "Fullscreen")
};

UENUM(BlueprintType)
enum class EG2IScreenResolution : uint8
{
	Resolution1280On720 UMETA(DisplayName = "1280x720"),
	Resolution1920On1080 UMETA(DisplayName = "1920x1080"),
	Resolution2560On1440 UMETA(DisplayName = "2560x1440"),
	Resolution3840On2160 UMETA(DisplayName = "3840x2160")
};

UENUM(BlueprintType)
enum class EG2IFrameRate : uint8
{
	FPS30 UMETA(DisplayName = "30 FPS"),
	FPS60 UMETA(DisplayName = "60 FPS"),
	FPS120 UMETA(DisplayName = "120 FPS"),
	FPSUnlimited UMETA(DisplayName = "Unlimited FPS")
};

UENUM(BlueprintType)
enum class EG2IQuality : uint8
{
	Low UMETA(DisplayName = "Low Quality"),
	Medium UMETA(DisplayName = "Medium Quality"),
	High UMETA(DisplayName = "High Quality"),
	Ultra UMETA(DisplayName = "Ultra Quality"),
	Epic UMETA(DisplayName = "Epic Quality")
};

UENUM(BlueprintType)
enum class EG2IAntialiasing : uint8
{
	None UMETA(DisplayName = "None"),
	FXAA UMETA(DisplayName = "FXAA"),
	TAA UMETA(DisplayName = "TAA"),
	MSAA UMETA(DisplayName = "MSAA"),
	TSR UMETA(DisplayName = "TSR")
};

/**
 * Options Defaults
 */
UCLASS()
class G2I_API UG2IOptionsParameters : public UDataAsset
{
	GENERATED_BODY()
	
public:

	// ==================== KEYBOARD AND MOUSE ====================
	float MouseSensitive = 1.f;
	bool bIsInvertedCameraHorizontalRotation = false;
	bool bIsInvertedCameraVerticalRotation = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyboard and mouse|MouseSensitive", meta = (ClampMin = "0.0"))
	float DefaultMouseSensitive = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyboard and mouse|MouseSensitive", meta = (ClampMin = "0.0"))
	float MinMouseSensitive = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyboard and mouse|MouseSensitive", meta = (ClampMin = "0.0"))
	float MaxMouseSensitive = 5.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyboard and mouse|MouseSensitive", meta = (ClampMin = "0.0"))
	float StepMouseSensitive = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyboard and mouse|Inversion")
	bool bIsDefaultInvertedCameraHorizontalRotation = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyboard and mouse|Inversion")
	bool bIsDefaultInvertedCameraVerticalRotation = false;

	// ==================== GRAPHICS ====================
	EG2IAntialiasing Antialiasing = EG2IAntialiasing::TSR;
	bool bCustomQuality = false;
	EG2IQuality OverallQuality = EG2IQuality::Epic;
	EG2IQuality TextureQuality = EG2IQuality::Epic;
	EG2IQuality ShadowsQuality = EG2IQuality::Epic;
	EG2IQuality EffectsQuality = EG2IQuality::Epic;
	EG2IQuality PostProcessingQuality = EG2IQuality::Epic;
	EG2IQuality AntialiasingQuality = EG2IQuality::Epic;
	EG2IQuality GlobalIlluminationQuality = EG2IQuality::Epic;
	EG2IQuality ReflectionQuality = EG2IQuality::Epic;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graphics")
	EG2IAntialiasing DefaultAntialiasing = EG2IAntialiasing::TSR;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graphics")
	bool bDefaultCustomQuality = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graphics", meta = (ToolTip = "Used only if bDefaultCustomQuality == false"))
	EG2IQuality DefaultOverallQuality = EG2IQuality::Epic;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graphics|Optional", meta = (ToolTip = "Used only if bDefaultCustomQuality == true"))
	EG2IQuality DefaultTextureQuality = EG2IQuality::Epic;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graphics|Optional", meta = (ToolTip = "Used only if bDefaultCustomQuality == true"))
	EG2IQuality DefaultShadowsQuality = EG2IQuality::Epic;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graphics|Optional", meta = (ToolTip = "Used only if bDefaultCustomQuality == true"))
	EG2IQuality DefaultEffectsQuality = EG2IQuality::Epic;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graphics|Optional", meta = (ToolTip = "Used only if bDefaultCustomQuality == true"))
	EG2IQuality DefaultPostProcessingQuality = EG2IQuality::Epic;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graphics|Optional", meta = (ToolTip = "Used only if bDefaultCustomQuality == true"))
	EG2IQuality DefaultAntialiasingQuality = EG2IQuality::Epic;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graphics|Optional", meta = (ToolTip = "Used only if bDefaultCustomQuality == true"))
	EG2IQuality DefaultGlobalIlluminationQuality = EG2IQuality::Epic;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graphics|Optional", meta = (ToolTip = "Used only if bDefaultCustomQuality == true"))
	EG2IQuality DefaultReflectionQuality = EG2IQuality::Epic;
	
	// ==================== SCREEN ====================
	EG2IScreenMode ScreenMode = EG2IScreenMode::WindowedFullscreen;
	EG2IScreenResolution ScreenResolution = EG2IScreenResolution::Resolution1920On1080;
	EG2IFrameRate FrameRate = EG2IFrameRate::FPSUnlimited;
	bool bIsVSyncOn = true;
	//float Brightness = 5.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Screen|View")
	EG2IScreenMode DefaultScreenMode = EG2IScreenMode::WindowedFullscreen;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Screen|View")
	EG2IScreenResolution DefaultScreenResolution = EG2IScreenResolution::Resolution1920On1080;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Screen|InGame")
	EG2IFrameRate DefaultFrameRate = EG2IFrameRate::FPSUnlimited;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Screen|InGame")
	bool bIsDefaultVSyncOn = true;
	
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Screen|InGame|Brightness", meta = (ClampMin = "0.0"))
	float DefaultBrightness = 5.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Screen|InGame|Brightness", meta = (ClampMin = "0.0"))
	float MinBrightness = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Screen|InGame|Brightness", meta = (ClampMin = "0.0"))
	float MaxBrightness = 10.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Screen|InGame|Brightness", meta = (ClampMin = "0.0"))
	float StepBrightness = 1.f;*/
	
	// ==================== SOUNDS ====================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float CommonVolume = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float MusicVolume = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float EffectsVolume = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float DialoguesVolume = 10.f;
	
private:
	
	UPROPERTY()
	TObjectPtr<UGameUserSettings> Settings;
	
public:
	
	void SetupDefaultParameters();
	
	void SetScreenMode(EG2IScreenMode NewScreenMode);
	void SetScreenResolution(EG2IScreenResolution NewScreenResolution);
	void SetFrameRate(EG2IFrameRate NewFrameRate);
	//void SetBrightness(float NewBrightness);
	void SetVSync(bool bIsNewVSync);
	
	void SetAntialiasing(EG2IAntialiasing NewAntialiasing);
	void SetOverallQuality(EG2IQuality NewQuality);
	void SetTextureQuality(EG2IQuality NewQuality);
	void SetShadowsQuality(EG2IQuality NewQuality);
	void SetEffectsQuality(EG2IQuality NewQuality);
	void SetPostProcessingQuality(EG2IQuality NewQuality);
	void SetAntialiasingQuality(EG2IQuality NewQuality);
	void SetGlobalIlluminationQuality(EG2IQuality NewQuality);
	void SetReflectionQuality(EG2IQuality NewQuality);
	
};