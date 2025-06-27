// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTT_MyFindRandomPatrolLocation.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTKMK_API UBTT_MyFindRandomPatrolLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


	//?????
	UPROPERTY(EditAnywhere, Category = "Value", BlueprintReadWrite)
	TArray<AActor*> NewVar;

	UPROPERTY(EditAnywhere, Category = "Value", BlueprintReadWrite)
	float PatrolSpeed = 125.0f;

	UPROPERTY(EditAnywhere, Category = "Value", BlueprintReadWrite)
	float PatrolRadius = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Key", BlueprintReadWrite)
	FBlackboardKeySelector PatrolLocation;

};
