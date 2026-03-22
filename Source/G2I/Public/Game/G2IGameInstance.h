#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "G2IGameInstance.generated.h"

class UG2IWidgetComponentParameters;
class UG2IStringTablesCatalog;
class UG2IWidgetsCatalog;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerControllerInitDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartLevelInitDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCloseLevelDelegate);

/**
 * Base game instance for G2I game
 * saves and loads gameplay & settings data
 */
UCLASS()
class G2I_API UG2IGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FPlayerControllerInitDelegate OnPlayerControllerInitDelegate;

	UPROPERTY(BlueprintAssignable)
	FStartLevelInitDelegate OnStartLevelInitDelegate;

	UPROPERTY(BlueprintAssignable)
	FCloseLevelDelegate OnCloseLevelDelegate;

protected:

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2IWidgetsCatalog> WidgetsCatalog;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2IStringTablesCatalog> StringTablesCatalog;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2IWidgetComponentParameters> WidgetComponentsParameters;

	UPROPERTY(EditAnywhere)
	FName MainMenuLevelName = "MainMenuLevel";
	
	UPROPERTY(EditAnywhere, meta=(ToolTip="List of levels in the editor in order without the main menu"))
	TArray<FName> LevelsNameInOrderInEditor;
	
	UPROPERTY(EditAnywhere, meta=(ToolTip="List of levels in the game in order without the main menu."))
	TArray<FName> LevelsNameInOrderInGame;

private:

#if WITH_EDITORONLY_DATA
	static constexpr int32 StartLevel = -1;
#else
	static constexpr int32 StartLevel = 0;
#endif
	
	int32 CurrentLevelIndex = StartLevel;

	FName CurrentLevelName = NAME_None;

public:

	virtual void Init() override;
	
	UG2IWidgetsCatalog *GetWidgetsCatalog();
	UG2IStringTablesCatalog *GetStringTablesCatalog();
	UG2IWidgetComponentParameters *GetWidgetComponentParameters();
	FName GetMainMenuLevelName() const;
	FName GetCurrentLevelName() const;

	bool IsMainMenuLevel() const;

	bool LoadLevelByIndex(uint32 Index);
	void LoadNextLevel();
	void LoadMainMenuLevel();

	void SetMainMenuLevelIndex();

protected:

	UFUNCTION()
	void StartLevelInitialize();

	void SetCurrentLevelName();
	
	bool OpenLevel(FName LevelName) const;

};
