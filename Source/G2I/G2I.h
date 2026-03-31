// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/** Main log category used across the project */
DECLARE_LOG_CATEGORY_EXTERN(LogG2I, Log, All);

namespace G2I
{
	void DebugErrorMessage(const FString& DebugMessage, const FColor& Color = FColor::Red, float Time = 5.f);
	void DebugWarningMessage(const FString& DebugMessage, const FColor& Color = FColor::Orange, float Time = 5.f);
	void DebugLogMessage(const FString& DebugMessage, const FColor& Color = FColor::Green, float Time = 5.f);

#if WITH_EDITORONLY_DATA
	void ScreenEditorMessage(const FString& DebugMessage, const FColor& Color, float Time = 5.f);
#endif
}