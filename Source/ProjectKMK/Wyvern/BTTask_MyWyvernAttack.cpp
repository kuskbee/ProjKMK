// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MyWyvernAttack.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "WyvernCharacter.h"

EBTNodeResult::Type UBTTask_MyWyvernAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    AWyvernCharacter* WyvernChar = Cast<AWyvernCharacter>(OwnerComp.GetAIOwner()->GetPawn());

    if (WyvernChar)
    {
        float ChaseSpeed = 500.0f;
        WyvernChar->UpdateWalkSpeed(ChaseSpeed);
        return EBTNodeResult::Succeeded;
    }
    else
    {
        return EBTNodeResult::Failed;
    }
}
