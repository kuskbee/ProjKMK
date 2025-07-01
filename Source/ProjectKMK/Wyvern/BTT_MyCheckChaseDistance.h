// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTT_MyCheckChaseDistance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTKMK_API UBTT_MyCheckChaseDistance : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Key", BlueprintReadWrite)
	FBlackboardKeySelector TargetActor;

	UPROPERTY(EditAnywhere, Category = "Value", BlueprintReadWrite)
	float Distance = 2000.0f;
};
