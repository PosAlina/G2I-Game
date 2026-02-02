#pragma once

#include "CoreMinimal.h"
#include "G2IGameInstance.h"
#include "G2IUIDisplayManager.h"
#include "G2IWidgetsCatalog.h"
#include "UObject/Object.h"
#include "G2IUIManager.generated.h"

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
};