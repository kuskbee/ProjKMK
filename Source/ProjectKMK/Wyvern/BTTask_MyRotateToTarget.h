// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_MyRotateToTarget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTKMK_API UBTTask_MyRotateToTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_MyRotateToTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnyWhere, Category = "Data", BlueprintReadWrite)
	float RotationSpeed = 5.0f;

	UPROPERTY(EditAnyWhere, Category = "Data", BlueprintReadWrite)
	float AcceptanceDegree = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Key", BlueprintReadWrite)
	FBlackboardKeySelector TargetActor;

};
