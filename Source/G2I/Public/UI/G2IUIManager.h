#pragma once

#include "CoreMinimal.h"
#include "G2IUIManager.generated.h"

class UG2IGameInstance;
enum class EG2IAimType : uint8;
class UG2IWidgetComponentParameters;
class UWidgetSwitcher;
class UPanelWidget;
class UG2IPropertyRow;
class UG2INumericalMultiValuePropertyRow;
class UListView;
class UG2ITextMultiValuePropertyRow;
class UInputAction;
enum class EG2IWidgetNames : uint8;
class UG2IUserWidget;
class UG2IWorldHintKeyWidgetComponent;
class UG2IUIDisplayManager;
class AG2IPlayerController;
class UG2IWorldHintWidgetComponent;

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
	FUIManagerInitialized OnUIManagerInitialized;
	
private:

	UPROPERTY()
	TObjectPtr<UG2IGameInstance> GameInstance;

	UPROPERTY()
	TObjectPtr<AG2IPlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<UG2IUIDisplayManager> DisplayManager;

	UPROPERTY()
	TObjectPtr<UG2IWidgetComponentParameters> WidgetComponentParameters;

	FDelegateHandle StartGameDelegateHandle;
	
public:
	// ==================== INITIALIZE ====================
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

protected:

	UFUNCTION()
	void InitializeInStartGame();
	void InitializeDefaultsInStartGame();
	
	UFUNCTION()
	void InitializeInStartLevel();
	void InitializeDefaultsInStartLevel();

	void PostInitializeDefaultsInStartGame() const;
	void PostInitializeDefaultsInStartLevel() const;
	void InitializeNewLevelUI() const;
	
	UFUNCTION()
	void CloseLevelUI();

public:

	// ==================== WIDGETS ====================
	UG2IUserWidget *CreateWidgetByName(EG2IWidgetNames WidgetName) const;
	void AddWidgetToPanel(UPanelWidget *Panel, EG2IWidgetNames WidgetName) const;
	void SwitchWidget(UWidgetSwitcher* Switcher, EG2IWidgetNames WidgetName) const;
	
	void OpenWidget(EG2IWidgetNames WidgetName, bool bIsFocus = true) const;
	void CloseWidget(EG2IWidgetNames WidgetName) const;

	void ShowWidget(EG2IWidgetNames WidgetName) const;
	void HideWidget(EG2IWidgetNames WidgetName) const;

	void OpenHUD() const;

	void ShowAllWidgets() const;

	void CloseAllWidgets() const;
	void CloseUI() const;

	FString GetWidgetNameString(EG2IWidgetNames WidgetName) const;

	// ==================== WORLD WIDGETS ====================
	void OpenWorldWidget(UG2IWorldHintWidgetComponent *WidgetComponent) const;
	void CloseWorldWidget(UG2IWorldHintWidgetComponent *WidgetComponent) const;
	bool CanSeeWorldWidget(UG2IWorldHintWidgetComponent *WidgetComponent) const;

	// ==================== AIMING WIDGET ====================
	void ChangeAimingType(EG2IAimType NewAimType) const;
	
	// ====================KEY HINT WIDGET ====================
	void SetKeyByInputAction(UG2IWorldHintWidgetComponent *WidgetComponent, UInputAction* InputAction, const TSubclassOf<APawn>& PawnClass) const;
	void SetKeyWidgetSize(UG2IWorldHintKeyWidgetComponent *WidgetComponent) const;
	
	// ==================== CONFIRMATION WIDGET ====================
	void SetupConfirmationWidget(const TFunction<void()>& NewConfirmAction, const TFunction<void()>& NewCancelAction,
		const FString& NewQuestionStringID = {},const FString& NewConfirmStringID = {},
		const FString& NewCancelStringID = {}) const;

	// ==================== LOADING WIDGET ====================
	void SetLoadingProgressPercent(float Percent) const;

	// ==================== CREATORS WIDGET ====================
	void SetupCreatorsWidget(const TFunction<void()>& NewBackAction) const;

	// ==================== OPTIONS WIDGETS ====================
	void SetupOptionsWidget(const TFunction<void()>& NewBackAction) const;
	void SetupControlsWidget(UWidgetSwitcher* CharacterControlsSwitcher) const;

	// ==================== OPTIONS PROPERTIES ====================
	void SetPropertyRow(UG2ITextMultiValuePropertyRow* PropertySelector, const FString& PropertyNameStringID,
					TArray<FString>& ValuesNamesStringID, int32 DefaultValueIndex = 0) const;
	void SetPropertyRow(UG2INumericalMultiValuePropertyRow* PropertySelector, const FString& PropertyNameStringID,
		float MinValue, float MaxValue, float Step, float DefaultValue, int32 DecimalPlaces) const;
	void ApplyPropertiesValues(TArray<UG2IPropertyRow*> Properties) const;
	void SavePropertiesValues(TArray<UG2IPropertyRow*> Properties) const;
	void ApplyPropertyValue(const UG2IPropertyRow* PropertyRow) const;
	void SavePropertyValue(const UG2IPropertyRow* PropertyRow) const;
	void SetActionControl(const FText& ActionName, const FText& KeyName, UListView* ControlsList) const;
};