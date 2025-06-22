// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "MyMonState.generated.h"
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

USTRUCT(BlueprintType)
struct FMyMonState : public FTableRowBase
{
	GENERATED_BODY()

public:
	FMyMonState()
	{
		Name = "",
		MaxHP = 10000.0f;
		WeaknessHP = 2000.0f;
		Phase = EPhase::FirstPhase;
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Name"), BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "MaxHP", MakeStructureDefaultValue = "10000.000000"), BlueprintReadWrite)
	float MaxHP;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "WeaknessHP", MakeStructureDefaultValue = "2000.000000"), BlueprintReadWrite)
	float WeaknessHP;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Phase", MakeStructureDefaultValue = "NewEnumerator0"), BlueprintReadWrite)
	EPhase Phase;

};
