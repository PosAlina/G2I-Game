#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "G2IUserWidget.generated.h"

class UG2IGameInstance;
class AG2IPlayerController;
class UG2IUIManager;

/**
 * The base class for all widgets
 */
UCLASS()
class G2I_API UG2IUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	UPROPERTY()
	TObjectPtr<UWorld> World;

	UPROPERTY()
	TObjectPtr<UG2IUIManager> UIManager;
	
	UPROPERTY()
	TObjectPtr<AG2IPlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<UG2IGameInstance> GameInstance;

public:

	virtual void NativeOnInitialized() override;

protected:
	
	UFUNCTION()
	virtual void InitializeAfterManagerLoading() {}

private:
	
	void InitializeUIManager();
	void InitializePlayerController();
	void InitializeGameInstance();

};