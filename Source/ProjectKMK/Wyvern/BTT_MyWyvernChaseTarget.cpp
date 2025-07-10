// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MyWyvernChaseTarget.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "WyvernCharacter.h"

EBTNodeResult::Type UBTT_MyWyvernChaseTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->
        GetValueAsObject(TEXT("TargetActor")));

    if (Target)
    {
        AWyvernCharacter* WyvernChar = Cast<AWyvernCharacter>(OwnerComp.GetAIOwner()->GetPawn());
        if (WyvernChar)
        {
            WyvernChar->UpdateWalkSpeed(ChaseSpeed);
            return EBTNodeResult::Succeeded;
        }
    }
    else
    {
        //// ���� UBTService_UpdateAttackTarget�� tick�� ������ ���̰�, ��׷� �ý����� �߰��ϸ�, ���⼭ TargetActor�� Is Not Valid ������ MonAIState�� Change ���� �ʿ䰡 ���µ�..
    }

    return EBTNodeResult::Failed;
}
