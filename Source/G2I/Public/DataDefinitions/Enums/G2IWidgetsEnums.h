#pragma once

UENUM(BlueprintType)
enum class EG2IWidgetNames : uint8
{
	Aim UMETA(DisplayName = "Aim widget"),
	Pause UMETA(DisplayName = "Pause widget"),
	TrainingScreen UMETA(DisplayName = "Screen for command's describing"),
};

UENUM(BlueprintType)
enum class EG2IUIType : uint8
{
	UI UMETA(DisplayName = "Focus on widget"),
	HUD UMETA(DisplayName = "Widget is not interactive")
};
