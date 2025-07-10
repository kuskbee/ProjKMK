// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_MyCheckAttackEnd.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTKMK_API UBTTask_MyCheckAttackEnd : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void EventEndAttackMontage(UAnimMontage* Montage, bool bInterrupted);

	UBehaviorTreeComponent* CachedOwnerComp;
};
