// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MyWyvernAttack.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "WyvernInterface.h"

EBTNodeResult::Type UBTTask_MyWyvernAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    IWyvernInterface* WyvernChar = Cast<IWyvernInterface>(OwnerComp.GetAIOwner()->GetPawn());

    if (WyvernChar)
    {
        WyvernChar->Attack();

        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;

}