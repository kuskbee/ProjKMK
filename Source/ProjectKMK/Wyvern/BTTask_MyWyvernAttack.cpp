// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MyWyvernAttack.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "WyvernCharacter.h"
#include "WyvernInterface.h"

EBTNodeResult::Type UBTTask_MyWyvernAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();

    IWyvernInterface* WyvernChar = Cast<IWyvernInterface>(ControlledPawn);
    if (WyvernChar)
    {
        WyvernChar->Attack();

        //OnAttackEnd Event Bind

        return EBTNodeResult::Succeeded;
    }
    else
    {
        return EBTNodeResult::Failed;
    }
}
