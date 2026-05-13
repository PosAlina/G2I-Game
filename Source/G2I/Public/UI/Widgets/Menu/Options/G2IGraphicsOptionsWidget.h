#pragma once

#include "CoreMinimal.h"
#include "G2IOptionsBaseSubWidget.h"
#include "G2IGraphicsOptionsWidget.generated.h"

class UG2IPropertyRow;
class UG2ITextMultiValuePropertyRow;

UCLASS()
class G2I_API UG2IGraphicsOptionsWidget : public UG2IOptionsBaseSubWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2ITextMultiValuePropertyRow> Antialiasing;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2ITextMultiValuePropertyRow> OverallQuality;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2ITextMultiValuePropertyRow> TextureQuality;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2ITextMultiValuePropertyRow> ShadowsQuality;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2ITextMultiValuePropertyRow> EffectsQuality;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2ITextMultiValuePropertyRow> PostProcessingQuality;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2ITextMultiValuePropertyRow> AntialiasingQuality;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2ITextMultiValuePropertyRow> GlobalIlluminationQuality;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2ITextMultiValuePropertyRow> ReflectionQuality;

public:

	virtual void ApplyOptions_Implementation() override;
	virtual void CancelUnAppliedOptions_Implementation() override;

protected:
	
	virtual void InitializeAfterManagerLoading() override;

private:

	void InitializeDefaults();
	void InitializeAntialiasing() const;
	void InitializeOverallQuality(const TArray<FString>& PropertiesQualityValue) const;
	void InitializeTextureQuality(TArray<FString>& PropertiesQualityValue) const;
	void InitializeShadowsQuality(TArray<FString>& PropertiesQualityValue) const;
	void InitializeEffectsQuality(TArray<FString>& PropertiesQualityValue) const;
	void InitializePostProcessingQuality(TArray<FString>& PropertiesQualityValue) const;
	void InitializeAntialiasingQuality(TArray<FString>& PropertiesQualityValue) const;
	void InitializeGlobalIlluminationQuality(TArray<FString>& PropertiesQualityValue) const;
	void InitializeReflectionQuality(TArray<FString>& PropertiesQualityValue) const;
	
	void BindDelegates();
	
	UFUNCTION()
	void SetPropertiesByQuality(const int32 Quality);

	void SetPropertyByQuality(UG2ITextMultiValuePropertyRow* Property, const int32 Quality) const;
};
