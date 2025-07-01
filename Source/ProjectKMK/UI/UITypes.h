#pragma once

#include "CoreMinimal.h"
#include "UITypes.generated.h"

UENUM(BlueprintType)
enum class EGameState : uint8
{
	EGS_None UMETA(DisplayName = "None"),
	EGS_Win UMETA(DisplayName = "Win"),
	EGS_Lose UMETA(DisplayName = "Lose"),
	EGS_Play UMETA(DisplayName = "Play"),
	EGS_Ready UMETA(DisplayName = "Ready")
};