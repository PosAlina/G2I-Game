#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2IWidgetNames.h"
#include "G2ICutSceneWidget.generated.h"

class UImage;
class UWidgetSwitcher;

/**
 * Base widget with UI of cutscenes
 */
UCLASS()
class G2I_API UG2ICutSceneWidget : public UG2IUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(Transient, meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> SheetsSwitcher;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SkipBarImage;

protected:

	UPROPERTY(EditAnywhere)
	EG2IWidgetNames CurrentWidgetName = EG2IWidgetNames::CutSceneStartBoilerRoom;

	UPROPERTY(EditAnywhere, Category = Skip)
	FKey SkipKey = EKeys::SpaceBar;

	UPROPERTY(EditAnywhere, Category = Switch)
	FKey NextKey = EKeys::LeftMouseButton;

	UPROPERTY(EditAnywhere, Category = Pause)
	FKey PauseKey = EKeys::Escape;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = Pause)
	FKey DebugPauseKey = EKeys::Enter;
#endif

private:

	float SkipSpeed = 1.f;
	
	bool bIsSkipPressed = false;

	bool bIsEnabled = true;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> SkipBarMaterialInstance;

protected:

	virtual void NativePreConstruct() override;

	virtual void InitializeAfterManagerLoading() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	void SetSkipPercent(float Percent) const;

	float GetSkipPercent() const;

	void CloseWidget() const;
	
};
