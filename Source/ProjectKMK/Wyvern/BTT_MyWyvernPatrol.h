// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_MyWyvernPatrol.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTKMK_API UBTT_MyWyvernPatrol : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnyWhere, Category = "Data", BlueprintReadWrite)
	float PatrolSpeed = 300.0f;

	UPROPERTY(EditAnyWhere, Category = "Data", BlueprintReadWrite)
	float PatrolRadius = 1000.0f;
};
