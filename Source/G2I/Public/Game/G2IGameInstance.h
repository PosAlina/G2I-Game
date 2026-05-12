#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "G2IGameInstance.generated.h"

class UG2ITasksCatalog;
class UG2IOptionsParameters;
class UG2ICutScenesParameters;
class UG2IWidgetComponentParameters;
class UG2IStringTablesCatalog;
class UG2IWidgetsCatalog;

UENUM(BlueprintType)
enum class EG2ILevelName : uint8
{
	None,
	TestLevel,
	BoilerRoom,
	ChildrenRoom,
	Hall,
	EndStory
};

USTRUCT(BlueprintType)
struct FG2ILevelInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> LevelAsset;

	UPROPERTY(EditAnywhere)
	int32 BackgroundIndex = 0;
};

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
	TObjectPtr<UG2ITasksCatalog> TasksCatalog;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2IWidgetComponentParameters> WidgetComponentsParameters;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2ICutScenesParameters> CutScenesParameters;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2IOptionsParameters> OptionsParameters;

	UPROPERTY(EditAnywhere)
	FG2ILevelInfo MainMenuLevelInfo;
	
	UPROPERTY(EditAnywhere)
	TMap<EG2ILevelName, FG2ILevelInfo> LevelsInfo;

	UPROPERTY(EditAnywhere, meta=(ToolTip="List of levels in the editor in order without the main menu"))
	TArray<EG2ILevelName> LevelsNameInOrderInEditor;
	
	UPROPERTY(EditAnywhere, meta=(ToolTip="List of levels in the game in order without the main menu."))
	TArray<EG2ILevelName> LevelsNameInOrderInGame;
	
private:

	int32 CurrentLevelIndex = -1;
	EG2ILevelName CurrentLevelEnum = EG2ILevelName::None;
	FString CurrentLevelName = "";
	FString MainMenuLevelName = "";

public:

	virtual void Init() override;
	
	UG2IWidgetsCatalog *GetWidgetsCatalog();
	UG2IStringTablesCatalog *GetStringTablesCatalog();
	UG2IWidgetComponentParameters *GetWidgetComponentParameters();
	UG2ICutScenesParameters *GetCutScenesParameters();
	UG2IOptionsParameters *GetOptionsParameters();
	UG2ITasksCatalog *GetTasksCatalog();
	FString GetMainMenuLevelName() const;
	FString GetCurrentLevelName() const;
	int32 GetIndex(const EG2ILevelName& LevelName) const;
	EG2ILevelName GetLevelEnum(const FString& LevelName) const;
	EG2ILevelName GetCurrentLevelEnum() const;

	bool IsMainMenuLevel() const;

	bool LoadLevel(const EG2ILevelName& LevelName);
	bool LoadLevel(uint32 Index);
	bool LoadNextLevel();
	bool LoadMainMenuLevel();

	void SetMainMenuLevelIndex();

protected:

	void StartLevelInitialize();

	void SetCurrentLevelInfo();
	
	bool OpenLevel(const TSoftObjectPtr<UWorld>& Level);
	bool LoadLevel(const FG2ILevelInfo& LevelInfo, uint32 Index);
};