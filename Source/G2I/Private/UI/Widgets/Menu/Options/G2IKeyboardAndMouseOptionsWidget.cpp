#include "Menu/Options/G2IKeyboardAndMouseOptionsWidget.h"
#include "G2I.h"
#include "G2IOptionsParameters.h"
#include "G2IUIManager.h"
#include "Menu/Elements/NumericalRow/G2INumericalMultiValuePropertyRow.h"
#include "Menu/Elements/TextRow/G2ITextMultiValuePropertyRow.h"

void UG2IKeyboardAndMouseOptionsWidget::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	InitializeDefaults();
}

void UG2IKeyboardAndMouseOptionsWidget::InitializeDefaults() const
{
	InitializeMouseSensitivity();
	InitializeVerticalMouseInversion();
	InitializeHorizontalMouseInversion();
}

void UG2IKeyboardAndMouseOptionsWidget::InitializeMouseSensitivity() const
{
	if (!ensure(MouseSensitivity))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Mouse Sensitivity\""), *GetName());
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
	
	const float DefaultValue = OptionsParameters->MouseSensitive;
	
	UIManager->SetPropertyRow(MouseSensitivity,"Mouse.Property.Sensitivity",
	0.f, 5.f, 0.1f, DefaultValue, 1);
	
	MouseSensitivity->OnApplyPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		OptionsParameters->MouseSensitive = MouseSensitivity->GetCurrentValue();
	};
	
	MouseSensitivity->OnCancelUnAppliedPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		const float DefaultOldValue = OptionsParameters->MouseSensitive;
		MouseSensitivity->SelectValue(DefaultOldValue);
	};
}

void UG2IKeyboardAndMouseOptionsWidget::InitializeVerticalMouseInversion() const
{
	if (!ensure(VerticalMouseInversion))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"VerticalMouseInversion\""), *GetName());
		return;
	}
	if (!ensure(OptionsParameters))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
			*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
		return;
	}
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	
	TArray<FString> PropertiesValue;
	PropertiesValue.Add("Property.Value.Off");
	PropertiesValue.Add("Property.Value.On");
	
	const int32 DefaultValue = OptionsParameters->bIsInvertedCameraVerticalRotation ? 1 : 0;
	
	UIManager->SetPropertyRow(VerticalMouseInversion, "Mouse.Property.VerticalInversion",
		PropertiesValue, DefaultValue);
	
	VerticalMouseInversion->OnApplyPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		OptionsParameters->bIsInvertedCameraVerticalRotation =
			VerticalMouseInversion->GetCurrentValueIndex() != 0.f;
	};
	
	VerticalMouseInversion->OnCancelUnAppliedPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		const int32 DefaultOldValue = OptionsParameters->bIsInvertedCameraVerticalRotation ? 1 : 0;
		VerticalMouseInversion->SelectValueByIndex(DefaultOldValue);
	};
}

void UG2IKeyboardAndMouseOptionsWidget::InitializeHorizontalMouseInversion() const
{
	if (!ensure(HorizontalMouseInversion))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"HorizontalMouseInversion\""), *GetName());
		return;
	}
	if (!ensure(OptionsParameters))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
			*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
		return;
	}
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	
	TArray<FString> PropertiesValue;
	PropertiesValue.Add("Property.Value.Off");
	PropertiesValue.Add("Property.Value.On");
	
	const int32 DefaultValue = OptionsParameters->bIsInvertedCameraHorizontalRotation ? 1 : 0;
	
	UIManager->SetPropertyRow(HorizontalMouseInversion, "Mouse.Property.HorizontalInversion",
		PropertiesValue, DefaultValue);
	
	HorizontalMouseInversion->OnApplyPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		OptionsParameters->bIsInvertedCameraHorizontalRotation =
			HorizontalMouseInversion->GetCurrentValueIndex() != 0.f;
	};

	HorizontalMouseInversion->OnCancelUnAppliedPropertyValue = [this]()
	{
		if (!ensure(OptionsParameters))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
				*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
			return;
		}
		const int32 DefaultOldValue = OptionsParameters->bIsInvertedCameraHorizontalRotation ? 1 : 0;
		HorizontalMouseInversion->SelectValueByIndex(DefaultOldValue);
	};
}

void UG2IKeyboardAndMouseOptionsWidget::ApplyOptions_Implementation()
{
	Super::ApplyOptions_Implementation();
	
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->ApplyPropertiesValues({MouseSensitivity, VerticalMouseInversion, HorizontalMouseInversion});
}

void UG2IKeyboardAndMouseOptionsWidget::CancelUnAppliedOptions_Implementation()
{
	Super::CancelUnAppliedOptions_Implementation();
	
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->CancelUnAppliedPropertiesValues({MouseSensitivity, VerticalMouseInversion, HorizontalMouseInversion});
}
