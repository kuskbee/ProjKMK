// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyDefine.h"
#include "Engine/DataTable.h"
#include "MyMonState.generated.h"
/**
 * 
 */

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
