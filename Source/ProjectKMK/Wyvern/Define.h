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
	None = 0 UMETA(DisplayName = "None"),
	Patrol = 1 UMETA(DisplayName = "Patrol"),
	Chase = 2 UMETA(DisplayName = "Chase"),
	Battle = 3 UMETA(DisplayName = "Battle"),
	Dead = 4 UMETA(DisplayName = "Dead"),
	Runaway = 5 UMETA(DisplayName = "Runaway"),
	RunawayReady = 6 UMETA(DisplayName = "RunawayReady")
};

USTRUCT(BlueprintType)
struct FMonsterSkill : public FTableRowBase
{
	GENERATED_BODY()

public:
	FMonsterSkill()
	{
		MonSkillName = "";
		Damage = 0.0f;
	}
	UPROPERTY(EditAnywhere, meta = (DisplayName = "MonSkillName", MakeStructureDefaultValue = "None"), BlueprintReadWrite)
	FString MonSkillName;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Damage", MakeStructureDefaultValue = "0.0"), BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "AnimMontage"), BlueprintReadWrite)
	TObjectPtr<UAnimMontage> AnimMontage;
};