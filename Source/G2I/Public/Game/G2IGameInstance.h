#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "G2IGameInstance.generated.h"

class UG2IWidgetComponentParameters;
class UG2IStringTablesCatalog;
class UG2IWidgetsCatalog;

DECLARE_MULTICAST_DELEGATE(FPlayerControllerInitDelegate);
DECLARE_MULTICAST_DELEGATE(FStartLevelInitDelegate);
DECLARE_MULTICAST_DELEGATE(FCloseLevelDelegate);

/**
 * Base game instance for G2I game
 * saves and loads gameplay & settings data
 */
UCLASS()
class G2I_API UG2IGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	FPlayerControllerInitDelegate OnPlayerControllerInitDelegate;

	FStartLevelInitDelegate OnStartLevelInitDelegate;

	FCloseLevelDelegate OnCloseLevelDelegate;

protected:

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2IWidgetsCatalog> WidgetsCatalog;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2IStringTablesCatalog> StringTablesCatalog;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2IWidgetComponentParameters> WidgetComponentsParameters;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> MainMenuLevel;
	
	UPROPERTY(EditAnywhere, meta=(ToolTip="List of levels in the editor in order without the main menu"))
	TArray<TSoftObjectPtr<UWorld>> LevelsNameInOrderInEditor;
	
	UPROPERTY(EditAnywhere, meta=(ToolTip="List of levels in the game in order without the main menu."))
	TArray<TSoftObjectPtr<UWorld>> LevelsNameInOrderInGame;

private:

#if WITH_EDITORONLY_DATA
	static constexpr int32 StartLevel = -1;
#else
	static constexpr int32 StartLevel = 0;
#endif
	
	int32 CurrentLevelIndex = StartLevel;

	FString CurrentLevelName = "";
	FString MainMenuLevelName = "";

public:

	virtual void Init() override;
	
	UG2IWidgetsCatalog *GetWidgetsCatalog();
	UG2IStringTablesCatalog *GetStringTablesCatalog();
	UG2IWidgetComponentParameters *GetWidgetComponentParameters();
	FString GetMainMenuLevelName() const;
	FString GetCurrentLevelName() const;

	bool IsMainMenuLevel() const;

	bool LoadLevelByIndex(uint32 Index);
	void LoadNextLevel();
	void LoadMainMenuLevel();

	void SetMainMenuLevelIndex();

protected:

	void StartLevelInitialize();

	void SetCurrentLevelName();
	
	bool OpenLevel(const TSoftObjectPtr<UWorld>& Level) const;

};
