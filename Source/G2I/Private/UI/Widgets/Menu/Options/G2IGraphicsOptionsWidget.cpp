#include "Menu/Options/G2IGraphicsOptionsWidget.h"
#include "G2I.h"
#include "G2IOptionsParameters.h"
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
	TArray<FString> PropertiesQualityValue;
	PropertiesQualityValue.Add("Graphics.Property.Quality.Value.Low"); //Index of quality: 0
	PropertiesQualityValue.Add("Graphics.Property.Quality.Value.Medium"); //Index of quality: 1
	PropertiesQualityValue.Add("Graphics.Property.Quality.Value.High"); //Index of quality: 2
	PropertiesQualityValue.Add("Graphics.Property.Quality.Value.Ultra"); //Index of quality: 3
	PropertiesQualityValue.Add("Graphics.Property.Quality.Value.Epic"); //Index of quality: 4
	
	InitializeOverallQuality(PropertiesQualityValue);
	InitializeTextureQuality(PropertiesQualityValue);
	InitializeShadowsQuality(PropertiesQualityValue);
	InitializeEffectsQuality(PropertiesQualityValue);
	InitializePostProcessingQuality(PropertiesQualityValue);
	InitializeAntialiasingQuality(PropertiesQualityValue);
	InitializeGlobalIlluminationQuality(PropertiesQualityValue);
	InitializeReflectionQuality(PropertiesQualityValue);
	
	InitializeAntialiasing();

	if (!ensure(OverallQuality))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Overall Quality\""), *GetName());
		return;
	}
	SetPropertiesByQuality(OverallQuality->GetCurrentValueIndex());
}

void UG2IGraphicsOptionsWidget::InitializeOverallQuality(const TArray<FString>& PropertiesQualityValue) const
{
	if (!ensure(OverallQuality))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Overall Quality\""), *GetName());
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
	

	int32 DefaultValue;
	if (OptionsParameters->bCustomQuality)
	{
		DefaultValue = 5;
	}
	else
	{
		DefaultValue =
			StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(OptionsParameters->OverallQuality));
	}
	
	TArray<FString> OverallPropertiesQualityValue = PropertiesQualityValue;
	OverallPropertiesQualityValue.Add("Graphics.Property.Quality.Value.Custom"); //Index of quality: 5
	UIManager->SetPropertyRow(OverallQuality, "Graphics.Property.OverallQuality",
		OverallPropertiesQualityValue, DefaultValue);
	
	OverallQuality->OnApplyPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}

		switch (OverallQuality->GetCurrentValueIndex())
		{
		case 0:
			OptionsParameters->SetOverallQuality(EG2IQuality::Low);
			break;
		case 1:
			OptionsParameters->SetOverallQuality(EG2IQuality::Medium);
			break;
		case 2:
			OptionsParameters->SetOverallQuality(EG2IQuality::High);
			break;
		case 3:
			OptionsParameters->SetOverallQuality(EG2IQuality::Ultra);
			break;
		case 4:
			OptionsParameters->SetOverallQuality(EG2IQuality::Epic);
			break;
		case 5:
			OptionsParameters->bCustomQuality = true;
			break;
		default: ;
		}
	};
	
	OverallQuality->OnCancelUnAppliedPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		int32 OldValue;
		if (OptionsParameters->bCustomQuality)
		{
			OldValue = 5;
		}
		else
		{
			OldValue = StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(OptionsParameters->OverallQuality));
		}
		OverallQuality->SelectValueByIndex(OldValue);
	};
}

void UG2IGraphicsOptionsWidget::InitializeTextureQuality(TArray<FString>& PropertiesQualityValue) const
{
	if (!ensure(TextureQuality))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"TextureQuality\""), *GetName());
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


	const int32 DefaultValue = StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(OptionsParameters->TextureQuality));
	UIManager->SetPropertyRow(TextureQuality, "Graphics.Property.TextureQuality",
			PropertiesQualityValue, DefaultValue);
	
	TextureQuality->OnApplyPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}

		switch (TextureQuality->GetCurrentValueIndex())
		{
		case 0:
			OptionsParameters->SetTextureQuality(EG2IQuality::Low);
			break;
		case 1:
			OptionsParameters->SetTextureQuality(EG2IQuality::Medium);
			break;
		case 2:
			OptionsParameters->SetTextureQuality(EG2IQuality::High);
			break;
		case 3:
			OptionsParameters->SetTextureQuality(EG2IQuality::Ultra);
			break;
		case 4:
			OptionsParameters->SetTextureQuality(EG2IQuality::Epic);
			break;
		default: ;
		}
	};
	
	TextureQuality->OnCancelUnAppliedPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		const int32 OldValue =
			StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(OptionsParameters->TextureQuality));
		TextureQuality->SelectValueByIndex(OldValue);
	};
}

void UG2IGraphicsOptionsWidget::InitializeShadowsQuality(TArray<FString>& PropertiesQualityValue) const
{
	if (!ensure(ShadowsQuality))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"ShadowsQuality\""), *GetName());
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


	const int32 DefaultValue = StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(OptionsParameters->ShadowsQuality));
	UIManager->SetPropertyRow(ShadowsQuality, "Graphics.Property.ShadowsQuality",
			PropertiesQualityValue, DefaultValue);
	
	ShadowsQuality->OnApplyPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}

		switch (ShadowsQuality->GetCurrentValueIndex())
		{
		case 0:
			OptionsParameters->SetShadowsQuality(EG2IQuality::Low);
			break;
		case 1:
			OptionsParameters->SetShadowsQuality(EG2IQuality::Medium);
			break;
		case 2:
			OptionsParameters->SetShadowsQuality(EG2IQuality::High);
			break;
		case 3:
			OptionsParameters->SetShadowsQuality(EG2IQuality::Ultra);
			break;
		case 4:
			OptionsParameters->SetShadowsQuality(EG2IQuality::Epic);
			break;
		default: ;
		}
	};
	
	ShadowsQuality->OnCancelUnAppliedPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		const int32 OldValue =
			StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(OptionsParameters->ShadowsQuality));
		ShadowsQuality->SelectValueByIndex(OldValue);
	};
}

void UG2IGraphicsOptionsWidget::InitializeEffectsQuality(TArray<FString>& PropertiesQualityValue) const
{
	if (!ensure(EffectsQuality))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"EffectsQuality\""), *GetName());
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


	const int32 DefaultValue = StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(OptionsParameters->EffectsQuality));
	UIManager->SetPropertyRow(EffectsQuality, "Graphics.Property.EffectsQuality",
			PropertiesQualityValue, DefaultValue);
	
	EffectsQuality->OnApplyPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}

		switch (EffectsQuality->GetCurrentValueIndex())
		{
		case 0:
			OptionsParameters->SetEffectsQuality(EG2IQuality::Low);
			break;
		case 1:
			OptionsParameters->SetEffectsQuality(EG2IQuality::Medium);
			break;
		case 2:
			OptionsParameters->SetEffectsQuality(EG2IQuality::High);
			break;
		case 3:
			OptionsParameters->SetEffectsQuality(EG2IQuality::Ultra);
			break;
		case 4:
			OptionsParameters->SetEffectsQuality(EG2IQuality::Epic);
			break;
		default: ;
		}
	};
	
	EffectsQuality->OnCancelUnAppliedPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		const int32 OldValue =
			StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(OptionsParameters->EffectsQuality));
		EffectsQuality->SelectValueByIndex(OldValue);
	};
}

void UG2IGraphicsOptionsWidget::InitializePostProcessingQuality(TArray<FString>& PropertiesQualityValue) const
{
	if (!ensure(PostProcessingQuality))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"PostProcessingQuality\""), *GetName());
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

	const int32 DefaultValue = StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(OptionsParameters->PostProcessingQuality));
	UIManager->SetPropertyRow(PostProcessingQuality, "Graphics.Property.PostProcessingQuality",
			PropertiesQualityValue, DefaultValue);
	
	PostProcessingQuality->OnApplyPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}

		switch (PostProcessingQuality->GetCurrentValueIndex())
		{
		case 0:
			OptionsParameters->SetPostProcessingQuality(EG2IQuality::Low);
			break;
		case 1:
			OptionsParameters->SetPostProcessingQuality(EG2IQuality::Medium);
			break;
		case 2:
			OptionsParameters->SetPostProcessingQuality(EG2IQuality::High);
			break;
		case 3:
			OptionsParameters->SetPostProcessingQuality(EG2IQuality::Ultra);
			break;
		case 4:
			OptionsParameters->SetPostProcessingQuality(EG2IQuality::Epic);
			break;
		default: ;
		}
	};
	
	PostProcessingQuality->OnCancelUnAppliedPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		const int32 OldValue =
			StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(OptionsParameters->PostProcessingQuality));
		PostProcessingQuality->SelectValueByIndex(OldValue);
	};
}

void UG2IGraphicsOptionsWidget::InitializeAntialiasingQuality(TArray<FString>& PropertiesQualityValue) const
{
	if (!ensure(AntialiasingQuality))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"AntialiasingQuality\""), *GetName());
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

	const int32 DefaultValue = StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(OptionsParameters->AntialiasingQuality));
	UIManager->SetPropertyRow(AntialiasingQuality, "Graphics.Property.AntialiasingQuality",
			PropertiesQualityValue, DefaultValue);
	
	AntialiasingQuality->OnApplyPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}

		switch (AntialiasingQuality->GetCurrentValueIndex())
		{
		case 0:
			OptionsParameters->SetAntialiasingQuality(EG2IQuality::Low);
			break;
		case 1:
			OptionsParameters->SetAntialiasingQuality(EG2IQuality::Medium);
			break;
		case 2:
			OptionsParameters->SetAntialiasingQuality(EG2IQuality::High);
			break;
		case 3:
			OptionsParameters->SetAntialiasingQuality(EG2IQuality::Ultra);
			break;
		case 4:
			OptionsParameters->SetAntialiasingQuality(EG2IQuality::Epic);
			break;
		default: ;
		}
	};
	
	AntialiasingQuality->OnCancelUnAppliedPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		const int32 OldValue =
			StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(OptionsParameters->AntialiasingQuality));
		AntialiasingQuality->SelectValueByIndex(OldValue);
	};
}

void UG2IGraphicsOptionsWidget::InitializeGlobalIlluminationQuality(TArray<FString>& PropertiesQualityValue) const
{
	if (!ensure(GlobalIlluminationQuality))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"GlobalIlluminationQuality\""), *GetName());
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

	const int32 DefaultValue = StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(OptionsParameters->GlobalIlluminationQuality));
	UIManager->SetPropertyRow(GlobalIlluminationQuality, "Graphics.Property.GlobalIlluminationQuality",
			PropertiesQualityValue, DefaultValue);
	
	GlobalIlluminationQuality->OnApplyPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}

		switch (GlobalIlluminationQuality->GetCurrentValueIndex())
		{
		case 0:
			OptionsParameters->SetGlobalIlluminationQuality(EG2IQuality::Low);
			break;
		case 1:
			OptionsParameters->SetGlobalIlluminationQuality(EG2IQuality::Medium);
			break;
		case 2:
			OptionsParameters->SetGlobalIlluminationQuality(EG2IQuality::High);
			break;
		case 3:
			OptionsParameters->SetGlobalIlluminationQuality(EG2IQuality::Ultra);
			break;
		case 4:
			OptionsParameters->SetGlobalIlluminationQuality(EG2IQuality::Epic);
			break;
		default: ;
		}
	};
	
	GlobalIlluminationQuality->OnCancelUnAppliedPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		const int32 OldValue =
			StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(OptionsParameters->GlobalIlluminationQuality));
		GlobalIlluminationQuality->SelectValueByIndex(OldValue);
	};
}

void UG2IGraphicsOptionsWidget::InitializeReflectionQuality(TArray<FString>& PropertiesQualityValue) const
{
	if (!ensure(ReflectionQuality))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"ReflectionQuality\""), *GetName());
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

	const int32 DefaultValue = StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(OptionsParameters->ReflectionQuality));
	UIManager->SetPropertyRow(ReflectionQuality, "Graphics.Property.ReflectionQuality",
			PropertiesQualityValue, DefaultValue);
	
	ReflectionQuality->OnApplyPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}

		switch (ReflectionQuality->GetCurrentValueIndex())
		{
		case 0:
			OptionsParameters->SetReflectionQuality(EG2IQuality::Low);
			break;
		case 1:
			OptionsParameters->SetReflectionQuality(EG2IQuality::Medium);
			break;
		case 2:
			OptionsParameters->SetReflectionQuality(EG2IQuality::High);
			break;
		case 3:
			OptionsParameters->SetReflectionQuality(EG2IQuality::Ultra);
			break;
		case 4:
			OptionsParameters->SetReflectionQuality(EG2IQuality::Epic);
			break;
		default: ;
		}
	};
	
	ReflectionQuality->OnCancelUnAppliedPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		const int32 OldValue =
			StaticEnum<EG2IQuality>()->GetIndexByValue(static_cast<int32>(OptionsParameters->ReflectionQuality));
		ReflectionQuality->SelectValueByIndex(OldValue);
	};
}

void UG2IGraphicsOptionsWidget::InitializeAntialiasing() const
{
	if (!ensure(Antialiasing))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"TAntialiasing\""), *GetName());
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
	PropertiesValue.Add("Graphics.Property.Antialiasing.Value.None");//Low quality of graphic
	PropertiesValue.Add("Graphics.Property.Antialiasing.Value.FXAA");//Medium quality of graphic
	PropertiesValue.Add("Graphics.Property.Antialiasing.Value.TAA"); //High quality of graphic
	PropertiesValue.Add("Graphics.Property.Antialiasing.Value.MSAA");//Epic quality of graphic
	PropertiesValue.Add("Graphics.Property.Antialiasing.Value.TSR");//Ultra quality of graphic

	const int32 DefaultValue = StaticEnum<EG2IAntialiasing>()->GetIndexByValue(static_cast<int32>(OptionsParameters->Antialiasing));
	UIManager->SetPropertyRow(Antialiasing, "Graphics.Property.Antialiasing",
			PropertiesValue, DefaultValue);
	
	Antialiasing->OnApplyPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}

		switch (Antialiasing->GetCurrentValueIndex())
		{
		case 0:
			OptionsParameters->SetAntialiasing(EG2IAntialiasing::None);
			break;
		case 1:
			OptionsParameters->SetAntialiasing(EG2IAntialiasing::FXAA);
			break;
		case 2:
			OptionsParameters->SetAntialiasing(EG2IAntialiasing::TAA);
			break;
		case 3:
			OptionsParameters->SetAntialiasing(EG2IAntialiasing::MSAA);
			break;
		case 4:
			OptionsParameters->SetAntialiasing(EG2IAntialiasing::TSR);
			break;
		default: ;
		}
	};
	
	Antialiasing->OnCancelUnAppliedPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		const int32 OldValue =
			StaticEnum<EG2IAntialiasing>()->GetIndexByValue(static_cast<int32>(OptionsParameters->Antialiasing));
		Antialiasing->SelectValueByIndex(OldValue);
	};
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

void UG2IGraphicsOptionsWidget::ApplyOptions_Implementation()
{
	Super::ApplyOptions_Implementation();
	
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->ApplyPropertiesValues({
		OverallQuality, TextureQuality, ShadowsQuality, PostProcessingQuality, Antialiasing});
}

void UG2IGraphicsOptionsWidget::CancelUnAppliedOptions_Implementation()
{
	Super::CancelUnAppliedOptions_Implementation();
	
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->CancelUnAppliedPropertiesValues(
		{OverallQuality, TextureQuality, ShadowsQuality, PostProcessingQuality, Antialiasing});
}

void UG2IGraphicsOptionsWidget::SetPropertiesByQuality(const int32 Quality)
{
	SetPropertyByQuality(TextureQuality, Quality);
	SetPropertyByQuality(ShadowsQuality, Quality);
	SetPropertyByQuality(EffectsQuality, Quality);
	SetPropertyByQuality(PostProcessingQuality, Quality);
	SetPropertyByQuality(AntialiasingQuality, Quality);
	SetPropertyByQuality(GlobalIlluminationQuality, Quality);
	SetPropertyByQuality(ReflectionQuality, Quality);
}

void UG2IGraphicsOptionsWidget::SetPropertyByQuality(UG2ITextMultiValuePropertyRow* Property, const int32 Quality) const
{
	if (!ensure(Property))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property"), *GetName());
		return;
	}
	
	if (Quality == 5)
	{
		Property->SetIsEnabled(true);
	}
	else
	{
		Property->SetIsEnabled(false);
		Property->SelectValueByIndex(Quality);
	}
}
