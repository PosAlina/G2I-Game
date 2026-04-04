#include "Menu/Options/G2IKeyboardAndMouseOptionsWidget.h"
#include "G2I.h"
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
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}

	if (ensure(MouseSensitivity))
	{
		UIManager->SetPropertyRow(MouseSensitivity,"Mouse.Property.Sensitivity",
			0.f, 5.f, 0.1f, 1.f, 1);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Overall Quality\""), *GetName());
	}
	if (ensure(MouseInversion))
	{
		TArray<FString> PropertiesValue;
		PropertiesValue.Add("Property.Value.Off");
		PropertiesValue.Add("Property.Value.On");
		UIManager->SetPropertyRow(MouseInversion, "Mouse.Property.Inversion",
			PropertiesValue, 0);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find widget of property \"Texture Quality\""), *GetName());
	}
}

void UG2IKeyboardAndMouseOptionsWidget::ApplyPropertiesValues() const
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->ApplyPropertiesValues({MouseSensitivity, MouseInversion});
}

void UG2IKeyboardAndMouseOptionsWidget::SavePropertiesValues() const
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->SavePropertiesValues({MouseSensitivity, MouseInversion});
}
