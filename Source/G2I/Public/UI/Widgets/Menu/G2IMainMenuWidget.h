#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2IMainMenuWidget.generated.h"

class UButton;
class UG2ISavingGameplayManager;

UCLASS()
class G2I_API UG2IMainMenuWidget : public UG2IUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ContinueButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NewGameButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> OptionsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CreatorsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> GalleryButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitGameButton;

protected:

	virtual void InitializeAfterManagerLoading() override;

	virtual void StartLevelInitialize() override;
	
	UFUNCTION()
	void OnNewGameButtonClicked();

	UFUNCTION()
	void OnOptionsButtonClicked();

	UFUNCTION()
	void OnCreatorsButtonClicked();

	UFUNCTION()
	void OnGalleryButtonClicked();

	UFUNCTION()
	void OnQuitGameButtonClicked();

	UFUNCTION()
	void OnContinueButtonClicked();

private:

	void NewGameWithSaveExists() const;
	void LoadNewGame() const;
	
	void InitializeDefaults() const;
	void BindDelegates();
	
	TFunction<void()> GetShowCurrentWidgetFunction() const;

private:

	UPROPERTY()
	TObjectPtr<UG2ISavingGameplayManager> SaveManager;
};
