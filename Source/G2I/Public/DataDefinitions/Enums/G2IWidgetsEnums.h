#pragma once

UENUM(BlueprintType)
enum class EG2IWidgetNames : uint8
{
	Aim UMETA(DisplayName = "Aim widget"),
	Pause UMETA(DisplayName = "Pause widget"),
	TrainingScreen UMETA(DisplayName = "Mechanic's description"),
	KeyHint UMETA(DisplayName = "Key hint"),
	PointHint UMETA(DisplayName = "Point hint (far key hint)")
};

UENUM(BlueprintType)
enum class EG2IUIType : uint8
{
	UI UMETA(DisplayName = "UI"),
	HUD UMETA(DisplayName = "HUD"),
	Widget3D UMETA(DisplayName = "3D widget")
};
