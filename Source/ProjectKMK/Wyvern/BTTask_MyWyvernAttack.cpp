// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MyWyvernAttack.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "MonsterInterface.h"

EBTNodeResult::Type UBTTask_MyWyvernAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    IMonsterInterface* WyvernChar = Cast<IMonsterInterface>(OwnerComp.GetAIOwner()->GetPawn());

    if (WyvernChar)
    {
        float AttackActionTime = WyvernChar->Attack();

        OwnerComp.GetBlackboardComponent()->SetValueAsFloat(GetSelectedBlackboardKey(), AttackActionTime);

        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}