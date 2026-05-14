#pragma once

UENUM(BlueprintType)
enum class EG2ITasksNames : uint8
{
	SwitchBetweenCharacters UMETA(DisplayName = "Training switching between characters"),
	ShootOnGear UMETA(DisplayName = "Training shooting - rotate gear"),
	PunchBoxes	UMETA(DisplayName = "Training punch - destruct boxes"),
	FlyUp UMETA(DisplayName = "Training fly up"),
	FollowAI UMETA(DisplayName = "Training to follow AI second character")
};
