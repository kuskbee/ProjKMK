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

    AWyvernCharacter* WyvernChar = Cast<AWyvernCharacter>(ControlledPawn);

    if (WyvernChar)
    {
        WyvernChar->EventAttackEnd.AddDynamic(this, &UBTTask_MyWyvernAttack::OnAttackEnd);
        WyvernChar->Attack();

        return EBTNodeResult::Succeeded;
    }
    else
    {
        return EBTNodeResult::Failed;
    }
}

void UBTTask_MyWyvernAttack::OnAttackEnd()
{
    if (ControlledPawn)
    {
        AWyvernCharacter* WyvernChar = Cast<AWyvernCharacter>(ControlledPawn);
        WyvernChar->EventAttackEnd.RemoveDynamic(this, &UBTTask_MyWyvernAttack::OnAttackEnd);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No Invaild Controlled Panw"));
    }
}
