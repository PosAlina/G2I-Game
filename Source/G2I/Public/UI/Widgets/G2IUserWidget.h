#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "G2IUserWidget.generated.h"

class UTextBlock;
class UCanvasPanel;
class UButton;
class UImage;

template<typename T>
struct FUIElementsMap
{
	UPROPERTY()
	TMap<FName, TObjectPtr<T>> Map;
};

/**
 * The base class for all widgets
 */
UCLASS()
class G2I_API UG2IUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> RootCanvasPanel;
	
	TTuple<FUIElementsMap<UImage>,
	FUIElementsMap<UButton>,
	FUIElementsMap<UTextBlock>> ActiveUIElements;

public:
	
	virtual bool Initialize() override;
	
	template<typename T>
	T *GetElement(FName Name);

private:
	
	template<typename T>
	bool CheckClassAndRegisterElement(FName UIElementName, void* UIElementPtr, UClass& PropertyClass);

	void ClearUIElements();

	template<typename T>
	void ClearUIElementsMap();

};

template <typename T>
T *UG2IUserWidget::GetElement(FName Name)
{
	FUIElementsMap<T> ElementsT = ActiveUIElements.Get<FUIElementsMap<T>>();
	if (TObjectPtr<T> *Element = ElementsT.Map.Find(Name))
	{
		if (*Element)
		{
			return *Element;
		}
	}
	return nullptr;
}

template <typename T>
bool UG2IUserWidget::CheckClassAndRegisterElement(FName UIElementName, void* UIElementPtr, UClass& PropertyClass)
{
	if (PropertyClass.IsChildOf(T::StaticClass()))
	{
		TObjectPtr<T>* PropertyPtr = reinterpret_cast<TObjectPtr<T>*>(UIElementPtr);
		if (PropertyPtr)
		{
			ActiveUIElements.Get<FUIElementsMap<T>>().Map.Add(UIElementName, *PropertyPtr);
		}
		return true;
	}
	return false;
}

template <typename T>
void UG2IUserWidget::ClearUIElementsMap()
{
	ActiveUIElements.Get<FUIElementsMap<T>>().Map.Empty();
}
