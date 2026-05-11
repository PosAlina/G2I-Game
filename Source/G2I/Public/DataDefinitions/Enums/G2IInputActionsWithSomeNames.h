#pragma once

UENUM(BlueprintType)
enum class EG2IInputActionsWithSomeNames : uint8
{
	MoveAction UMETA(DisplayName = "Move"),
	MoveSlider UMETA(DisplayName = "MoveSlider")
};