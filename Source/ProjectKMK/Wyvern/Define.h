// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Define.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EPhase : uint8
{
	FirstPhase = 0 UMETA(DisplayName = "FirstPhase"),
	SecondPhase = 1 UMETA(DisplayName = "SecondPhase"),
	ThirdPhase = 2 UMETA(DisplayName = "ThirdPhase")
};

UENUM(Blueprinttype)
enum class EAIState : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Patrol = 1 UMETA(DisplayName = "Patrol"),
	Chase = 2 UMETA(DisplayName = "Chase"),
	Battle = 3 UMETA(DisplayName = "Battle"),
	Dead = 4 UMETA(DisplayName = "Dead"),
	Runaway = 5 UMETA(DisplayName = "Runaway"),
	RunawayReady = 6 UMETA(DisplayName = "RunawayReady")
};