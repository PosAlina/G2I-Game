#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "G2IGameInstance.generated.h"

class UG2ICutScenesParameters;
struct FStreamableHandle;
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
	Hall
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
	TObjectPtr<UG2IWidgetComponentParameters> WidgetComponentsParameters;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2ICutScenesParameters> CutScenesParameters;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> MainMenuLevel;
	
	UPROPERTY(EditAnywhere)
	TMap<EG2ILevelName, TSoftObjectPtr<UWorld>> Levels;

	UPROPERTY(EditAnywhere, meta=(ToolTip="List of levels in the editor in order without the main menu"))
	TArray<EG2ILevelName> LevelsNameInOrderInEditor;
	
	UPROPERTY(EditAnywhere, meta=(ToolTip="List of levels in the game in order without the main menu."))
	TArray<EG2ILevelName> LevelsNameInOrderInGame;

	FTimerHandle LoadingTimerHandle;
	TSharedPtr<FStreamableHandle> LoadingLevelStreamingHandle;
	
private:

	int32 CurrentLevelIndex = -1;
	EG2ILevelName CurrentLevelEnum = EG2ILevelName::None;
	FString CurrentLevelName = "";
	FString MainMenuLevelName = "";

	// TODO: Tremp before screen loading
	float TimeCount = 0.f;
	const float MaxTimeCount = 5.f;

public:

	virtual void Init() override;
	
	UG2IWidgetsCatalog *GetWidgetsCatalog();
	UG2IStringTablesCatalog *GetStringTablesCatalog();
	UG2IWidgetComponentParameters *GetWidgetComponentParameters();
	UG2ICutScenesParameters *GetCutScenesParameters();
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
	bool LoadLevel(const TSoftObjectPtr<UWorld>& Level, uint32 Index);

	bool LoadScreenLoading() const;
	void UpdateLoadingProgressFixTime(const FName LevelName); //TODO: Change when set LoadingScreen By Time
	void UpdateLoadingProgress(const FName LevelName);
	void FinishLoading(FName LevelName);
};