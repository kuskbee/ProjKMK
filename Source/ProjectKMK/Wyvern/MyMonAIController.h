// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyMonAIController.generated.h"

/**
 * 
 */

UENUM(Blueprinttype)
enum class EAIState : uint8
{
	None		= 0 UMETA(DisplayName = "None"),
	Patrol		= 1 UMETA(DisplayName = "Patrol"),
	Chase		= 2 UMETA(DisplayName = "Chase"),
	Battle		= 3 UMETA(DisplayName = "Battle"),
	Dead		= 4 UMETA(DisplayName = "Dead"),
	Runaway		= 5 UMETA(DisplayName = "Runaway"),
	RunawayReady = 6 UMETA(DisplayName = "RunawayReady")
};

UCLASS()
class PROJECTKMK_API AMyMonAIController : public AAIController
{
	GENERATED_BODY()
	
};
