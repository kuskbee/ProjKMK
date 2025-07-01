// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_MyWyvernAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTKMK_API UBTTask_MyWyvernAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void OnAttackEnd();

	UPROPERTY(EditAnywhere, Category = "VALUE", BlueprintReadWrite)
	TObjectPtr<APawn> ControlledPawn;
};
