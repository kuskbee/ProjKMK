// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_MyWyvernChaseTarget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTKMK_API UBTT_MyWyvernChaseTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Data", BlueprintReadWrite)
	float ChaseSpeed = 500.0f;
};
