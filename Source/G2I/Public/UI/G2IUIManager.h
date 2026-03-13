#pragma once

#include "CoreMinimal.h"
#include "G2IUIManager.generated.h"

enum class EG2IAimType : uint8;
class UG2IWidgetComponentParameters;
enum class EG2IStringTablesTypes : uint8;
class UWidgetSwitcher;
class UPanelWidget;
class UG2IPropertyRow;
class UG2ITextMultiValuePropertyRow;
class UInputAction;
enum class EG2IWidgetNames : uint8;
class UG2IUserWidget;
class UG2IWorldHintKeyWidgetComponent;
class UG2IWidgetsCatalog;
class UG2IUIDisplayManager;
class AG2IPlayerController;
class UG2IGameInstance;
class UG2IWorldHintWidgetComponent;
class AG2IPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerControllerInitDelegate, APlayerController*, PlayerController);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUIManagerInitialized);

/**
 * Handler for all widget request
 */
UCLASS()
class G2I_API UG2IUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FPlayerControllerInitDelegate OnPlayerControllerInitDelegate;

	UPROPERTY(BlueprintAssignable)
	FUIManagerInitialized OnUIManagerInitialized;
	
private:

	UPROPERTY()
	TObjectPtr<AG2IPlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<UG2IUIDisplayManager> DisplayManager;

	UPROPERTY()
	TObjectPtr<UG2IWidgetComponentParameters> WidgetComponentParameters;
	
public:
	// ==================== INITIALIZE ====================
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION()
	void InitializeComponents(APlayerController* InPlayerController);

	void OpenHUD() const;

	// ==================== WIDGETS ====================
	UG2IUserWidget *CreateWidgetByName(EG2IWidgetNames WidgetName) const;
	void AddWidgetToPanel(UPanelWidget *Panel, EG2IWidgetNames WidgetName) const;
	void SwitchWidget(UWidgetSwitcher* Switcher, EG2IWidgetNames WidgetName) const;
	
	void OpenWidget(EG2IWidgetNames WidgetName) const;
	void CloseWidget(EG2IWidgetNames WidgetName) const;

	void ShowWidget(EG2IWidgetNames WidgetName) const;
	void HideWidget(EG2IWidgetNames WidgetName) const;

	void CloseAllWidgets() const;

	// ==================== WORLD WIDGETS ====================
	void OpenWorldWidget(UG2IWorldHintWidgetComponent *WidgetComponent) const;
	void CloseWorldWidget(UG2IWorldHintWidgetComponent *WidgetComponent) const;
	bool CanSeeWorldWidget(UG2IWorldHintWidgetComponent *WidgetComponent) const;

	// ==================== AIMING WIDGET ====================
	void ChangeAimingType(EG2IAimType NewAimType) const;
	
	// ====================KEY HINT WIDGET ====================
	void SetKeyByInputAction(UG2IWorldHintWidgetComponent *WidgetComponent, UInputAction* InputAction) const;
	void SetKeyWidgetSize(UG2IWorldHintKeyWidgetComponent *WidgetComponent) const;

private:
	
	FString GetWidgetNameString(EG2IWidgetNames WidgetName) const;
	
	// ==================== OPTIONS PROPERTIES ====================
	void SetPropertyRow(UG2ITextMultiValuePropertyRow* PropertySelector, const FString& PropertyNameStringID,
					TArray<FString>& ValuesNamesStringID, int32 DefaultValueIndex = 0) const;
	void ApplyPropertiesValues(TArray<UG2IPropertyRow*> Properties) const;
	void SavePropertiesValues(TArray<UG2IPropertyRow*> Properties) const;
	void ApplyPropertyValue(const UG2IPropertyRow* PropertyRow) const;
	void SavePropertyValue(const UG2IPropertyRow* PropertyRow) const;
};