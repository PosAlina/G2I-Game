#pragma once

#include "CoreMinimal.h"
#include "G2I.h"
#include "G2IWidgetsEnums.h"
#include "Widgets/G2IUserWidget.h"
#include "UObject/Object.h"
#include "G2IUIDisplayManager.generated.h"

class AG2IPlayerController;
struct FG2IWidgetClassesInfo;
class UG2IWidgetsCatalog;

USTRUCT()
struct FG2IWidgetInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UG2IUserWidget> Widget;
	
	EG2IUIType Type = EG2IUIType::HUD;
};

/**
 * All generic work with registered G2IUserWidget-s
 */
UCLASS()
class G2I_API UG2IUIDisplayManager : public UObject
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	TMap<EG2IWidgetNames, FG2IWidgetInfo> AllWidgets;

private:

	UPROPERTY()
	TObjectPtr<UWorld> World;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;
	
public:

	// ==================== UI ELEMENTS ====================
	void SetImage(const EG2IWidgetNames WidgetName, const FName ElementName, UTexture2D *Texture);

	void SetText(const EG2IWidgetNames WidgetName, const FName ElementName, const FString Str);

	template<typename T>
	void FullHideElement(const EG2IWidgetNames WidgetName, const FName ElementName);

	template<typename T>
	void ShowHiddenElement(const EG2IWidgetNames WidgetName, const FName ElementName);

public:
	// ==================== BASE FUNCTIONS ====================
	void Initialize(UG2IWidgetsCatalog* WidgetsCatalog);

	UG2IUserWidget *GetWidget(const EG2IWidgetNames WidgetName);

	void ShowWidget(const EG2IWidgetNames WidgetName);

	void HideWidget(const EG2IWidgetNames WidgetName);

	template<typename T>
	T *GetUIElement(const EG2IWidgetNames WidgetName, const FName ElementName);

private:

	void RegisterWidget(const EG2IWidgetNames WidgetName, FG2IWidgetClassesInfo WidgetClassInfo);
	
	UFUNCTION()
	void SetupDefaults();
	
};

template <typename T>
T *UG2IUIDisplayManager::GetUIElement(const EG2IWidgetNames WidgetName, const FName ElementName)
{
	if (UG2IUserWidget *Widget = GetWidget(WidgetName))
	{
		T *UIElement = Widget->GetElement<T>(ElementName);
		return UIElement;
	}
	return nullptr;
}

template <typename T>
void UG2IUIDisplayManager::FullHideElement(const EG2IWidgetNames WidgetName, const FName ElementName)
{
	if (T *Element = GetUIElement<T>(WidgetName, ElementName))
	{
		Element->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("Element doesn't exist in %s"), *GetName());
		return;
	}
}

template <typename T>
void UG2IUIDisplayManager::ShowHiddenElement(const EG2IWidgetNames WidgetName, const FName ElementName)
{
	if (T *Element = GetUIElement<T>(WidgetName, ElementName))
	{
		Element->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("Element doesn't exist in %s"), *GetName());
		return;
	}
}
