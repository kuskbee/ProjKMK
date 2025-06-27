// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "Animation/AnimMontage.h"

#include "ST_MyMonsterSkill.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FST_MyMonsterSkill : public FTableRowBase
{
	GENERATED_BODY();

public:
	FST_MyMonsterSkill()
	{
		MonSkillName = "None";
		Damage = 0.0f;
		SkillAnimMontage = nullptr;
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "MonSkillName", MakeStructureDefaultValue = "None"), BlueprintReadWrite)
	FString MonSkillName;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Damage", MakeStructureDefaultValue = "0.0"), BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "SkillAnimMontage"), BlueprintReadWrite)
	UAnimMontage* SkillAnimMontage;
};
