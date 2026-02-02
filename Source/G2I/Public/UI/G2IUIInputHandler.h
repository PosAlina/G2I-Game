#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "G2IUIInputHandler.generated.h"

class AG2IPlayerController;
class UG2IUIManager;
class UG2IUIDisplayManager;

/**
 * Handler for all events occuring in registered in Display Manager widgets
 */
UCLASS()
class G2I_API UG2IUIInputHandler : public UObject
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TObjectPtr<UG2IUIManager> UIManager;

	UPROPERTY()
	TObjectPtr<UG2IUIDisplayManager> DisplayManager;

	UPROPERTY()
	TObjectPtr<AG2IPlayerController> PlayerController;
	
public:

	void Initialize(UG2IUIManager *InUIManager, UG2IUIDisplayManager *InDisplayManager);
	
private:

	UFUNCTION()
	void SetupDefaults();
	
	void BindDelegates();
	
	UFUNCTION()
	void ContinueGame();

	UFUNCTION()
	void QuitGame();
	
};
