// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
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

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Patrol =	0 UMETA(DisplayName = "Patrol"),
	Chase =		1 UMETA(DisplayName = "Chase"),
	Battle =	2 UMETA(DisplayName = "Battle"),
	Dead =		3 UMETA(DisplayName = "Dead"),
	Runaway =	4 UMETA(DisplayName = "Runaway"),
	RunawayReady = 5 UMETA(DisplayName = "RunawayReady")
};