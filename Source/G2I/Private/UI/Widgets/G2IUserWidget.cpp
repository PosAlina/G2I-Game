#include "Widgets/G2IUserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"

bool UG2IUserWidget::Initialize()
{
	const bool bInitializeSuccess = Super::Initialize();
	ClearUIElements();

	for (TFieldIterator<FProperty> PropertyIterator(GetClass()); PropertyIterator; ++PropertyIterator)
	{
		if (const FObjectProperty *UIElement = CastField<FObjectProperty>(*PropertyIterator))
		{
			const FName UIElementName = (*PropertyIterator)->GetFName();
			void *UIElementPtr = UIElement->ContainerPtrToValuePtr<void>(this);

			if (!UIElement->PropertyClass)
			{
				continue;
			}
			
			if (CheckClassAndRegisterElement<UImage>(UIElementName, UIElementPtr, *UIElement->PropertyClass))
			{
				continue;
			}

			if (CheckClassAndRegisterElement<UButton>(UIElementName, UIElementPtr, *UIElement->PropertyClass))
			{
				continue;
			}

			if (CheckClassAndRegisterElement<UTextBlock>(UIElementName, UIElementPtr, *UIElement->PropertyClass))
			{
				continue;
			}
		}
	}
	
	return bInitializeSuccess;
}

void UG2IUserWidget::ClearUIElements()
{
	ClearUIElementsMap<UImage>();
	ClearUIElementsMap<UButton>();
	ClearUIElementsMap<UTextBlock>();
}
