#pragma once

#include "CoreMinimal.h"
#include "G2IAimTypeEnum.h"
#include "G2IUIManager.generated.h"

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
class UG2IUIInputHandler;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerControllerInitDelegate, APlayerController*, PlayerController);

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
	
private:

	UPROPERTY()
	TObjectPtr<UG2IGameInstance> GameInstance;

	UPROPERTY()
	TObjectPtr<AG2IPlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<AG2IPlayerState> PlayerState;

	UPROPERTY()
	TObjectPtr<UG2IUIDisplayManager> DisplayManager;

	UPROPERTY()
	TObjectPtr<UG2IUIInputHandler> UIHandler;

	UPROPERTY()
	TObjectPtr<UG2IWidgetsCatalog> WidgetsCatalog;
	
public:
	// ==================== INITIALIZE ====================
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION()
	void InitializeComponents(APlayerController* InPlayerController);
	void InitializeDefaultsWidgets();

	void OpenHUD() const;

	// ==================== WIDGETS ====================
	UG2IUserWidget *CreateWidgetByName(EG2IWidgetNames WidgetName) const;
	
	// ==================== WORLD WIDGETS ====================
	void OpenWorldWidget(UG2IWorldHintWidgetComponent *WidgetComponent) const;
	void CloseWorldWidget(UG2IWorldHintWidgetComponent *WidgetComponent) const;
	bool CanSeeWorldWidget(UG2IWorldHintWidgetComponent *WidgetComponent) const;

	// ==================== AIMING WIDGET ====================
	void InitializeAimingWidget() const;
	void OpenAimingWidget() const;
	void CloseAimingWidget() const;
	void ChangeAimingType(EG2IAimType NewAimType) const;

	// ==================== PAUSE WIDGET ====================
	void OpenPauseWidget() const;
	void ClosePauseWidget() const;
	
	// ==================== TRAINING SCREEN WIDGET ====================
	void InitializeTrainingScreenWidget();
	void OpenTrainingWidget() const;
	void SetCommonMechanicsDescription() const;

	UFUNCTION()
	void SetMechanicsDescriptionByCharacter(APawn *Pawn);
	
	// ====================KEY HINT WIDGET ====================
	void SetKeyByInputAction(UG2IWorldHintWidgetComponent *WidgetComponent, UInputAction* InputAction) const;
	void SetKeyWidgetSize(UG2IWorldHintKeyWidgetComponent *WidgetComponent) const;
};