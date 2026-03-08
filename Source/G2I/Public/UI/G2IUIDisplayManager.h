#pragma once

#include "CoreMinimal.h"
#include "G2I.h"
#include "G2IWidgetNames.h"
#include "G2IUserWidget.h"
#include "G2IWidgetTypes.h"
#include "G2IUIDisplayManager.generated.h"

enum class EG2IStringTablesTypes : uint8;
class UG2IWorldHintWidgetComponent;
class AG2IPlayerController;
struct FG2IWidgetClassesInfo;
class UG2IWidgetsCatalog;

USTRUCT()
struct FG2IWidgetInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UG2IUserWidget> Widget;
	
	EG2IWidgetTypes Type = EG2IWidgetTypes::UI;
};


template<typename T>
concept HasSetText = requires(T* UIElement, const FText& Text)
{
	UIElement->SetText(Text);
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

	UPROPERTY()
	TMap<uint32, TObjectPtr<UG2IWorldHintWidgetComponent>> AllWidgetComponents;
	
	TSet<uint32> ActiveWidgetComponentsID;

	TMap<EG2IWidgetTypes, TSet<EG2IWidgetNames>> AllActiveWidgetsNames;

private:

	UPROPERTY()
	TObjectPtr<UWorld> World;

	UPROPERTY()
	TObjectPtr<AG2IPlayerController> PlayerController;

	FCollisionQueryParams QueryParamsForWorldWidgetsActivate;

	FText MissingStringInStringTable = FText::FromString("<MISSING STRING TABLE ENTRY>");
	FText OverrideMissingStringInStringTable = FText::GetEmpty();

	TMap<EG2IStringTablesTypes, FName> StringTablesNames;
	
	// ==================== UI ELEMENTS ====================
public:

	template<typename T>
	requires HasSetText<T>
	void SetText(T *UIElement, EG2IStringTablesTypes StringTableType, const FString& KeyNewText,
		const FString& KeyDefaultText);

	FText GetText(EG2IStringTablesTypes StringTableType, const FString& KeyNewText);
	
private:

	bool IsMissingOrEmptyString(const FText& Text) const;
	
	// ==================== BASE WIDGET FUNCTIONS ====================
public:
	void Initialize();
	void InitializeGameInstanceDefaults();

	UG2IUserWidget *GetWidget(const EG2IWidgetNames WidgetName);

	UG2IUserWidget *CreateNewWidget(const EG2IWidgetNames WidgetName);

	void OpenWidget(const EG2IWidgetNames WidgetName);
	void CloseWidget(const EG2IWidgetNames WidgetName);

	void CloseAllActiveWidgets();
	void CloseActiveWidgetsByType(const EG2IWidgetTypes WidgetsType);

private:

	void RegisterWidget(const EG2IWidgetNames WidgetName, const FG2IWidgetClassesInfo& WidgetClassInfo);
	
	void SetupDefaults();
	void BindDelegates();

	UFUNCTION()
	void UpdateBindingDelegatesForChangedPawn(APawn *Pawn);
	
	// ==================== BASE WIDGET COMPONENTS FUNCTIONS ====================
public:

	void ShowWorldWidget(UG2IWorldHintWidgetComponent& WidgetComponent);

	void HideWorldWidget(UG2IWorldHintWidgetComponent& WidgetComponent);

	bool IsVisibleWorldWidget(UG2IWorldHintWidgetComponent& WidgetComponent) const;
	
private:
	
	void RegisterWorldWidgetComponent(UG2IWorldHintWidgetComponent& WidgetComponent);
	
	UFUNCTION()
	void ReactActiveWidgetComponentsToNewCameraLocation(const FVector& NewCameraLocation);

	static uint32 GetWorldWidgetComponentID(UG2IWorldHintWidgetComponent& WidgetComponent);
	
};

template <typename T> requires HasSetText<T>
void UG2IUIDisplayManager::SetText(T* UIElement, const EG2IStringTablesTypes StringTableType,
	const FString& KeyNewText, const FString& KeyDefaultText)
{
	if (!ensure(UIElement))
	{
		UE_LOG(LogG2I, Warning, TEXT("UI element with text is undefined in %s"), *GetName());
		return;
	}

	const FName *StringTableName = StringTablesNames.Find(StringTableType);
	if (!ensure(StringTableName))
	{
		UE_LOG(LogG2I, Warning, TEXT("Couldn't find string table with type %s in %s"),
			*StringTableName->ToString(), *GetName());
		return;
	}
	FText NewText = FText::FromStringTable((*StringTableName), KeyNewText);
	if (IsMissingOrEmptyString(NewText))
	{
		NewText = FText::FromStringTable((*StringTableName), KeyDefaultText);
	}
	UIElement->SetText(NewText);
}