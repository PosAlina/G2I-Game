#pragma once

UENUM(BlueprintType)
enum class EG2IWidgetNames : uint8
{
	Aim UMETA(DisplayName = "Aim widget"),
	Pause UMETA(DisplayName = "Pause widget"),
	TrainingScreen UMETA(DisplayName = "Mechanic's description"),
	KeyHint UMETA(DisplayName = "Key hint"),
	PointHint UMETA(DisplayName = "Point hint (far key hint)"),
	MainMenu UMETA(DisplayName = "Main widget"),
	Confirmation UMETA(DisplayName = "Confitmation widget"),
	Options UMETA(DisplayName = "Options widget"),
	Creators UMETA(DisplayName = "Creators widget"),
	SoundOptions UMETA(DisplayName = "Sound options widget"),
	CommonOptions UMETA(DisplayName = "Common options widget"),
	ControlsOptions UMETA(DisplayName = "Controls options widget"),
	ScreenOptions UMETA(DisplayName = "Screen options widget"),
	GraphicsOptions UMETA(DisplayName = "Graphics options widget"),
	OptionsPropertySelector UMETA(DisplayName = "Property values selector widget"),
	EngineerControlsOptions UMETA(DisplayName = "Controls options of engineer widget"),
	DaughterControlsOptions UMETA(DisplayName = "Controls options of daughter widget"),
	ControlRow UMETA(DisplayName = "Row of control's option")
};