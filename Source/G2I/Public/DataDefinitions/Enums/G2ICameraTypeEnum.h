#pragma once

UENUM(BlueprintType)
enum class EG2ICameraTypeEnum : uint8
{
	ThirdPersonCamera UMETA(DisplayName = "Third-Person Camera"),
	FixedCamera UMETA(DisplayName = "Fixed Camera")
};