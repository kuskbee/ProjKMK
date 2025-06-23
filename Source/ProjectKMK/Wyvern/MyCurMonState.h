// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MyMonState.h"
#include "Define.h"
#include "MyCurMonState.generated.h"
/**
 * 
 */

USTRUCT(BlueprintType)
struct FMyCurMonState
{
	GENERATED_BODY()

public:
	FMyCurMonState()
	{
		MonData.Name = "";
		MonData.MaxHP = 0.0f;
		MonData.WeaknessHP = 0.0f;
		MonData.Phase = EPhase::FirstPhase;
		CurHP = 0.0f;
		CurWeakHP = 0.0f;
	}
	UPROPERTY(EditAnywhere, Category = "Data", BlueprintReadWrite)
	FMyMonState MonData;

	UPROPERTY(EditAnywhere, Category = "Data", BlueprintReadWrite)
	float CurHP;

	UPROPERTY(EditAnywhere, Category = "Data", BlueprintReadWrite)
	float CurWeakHP;
};