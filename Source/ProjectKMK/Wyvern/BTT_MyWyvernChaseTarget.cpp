// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MyWyvernChaseTarget.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "WyvernCharacter.h"

EBTNodeResult::Type UBTT_MyWyvernChaseTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);


    AWyvernCharacter* WyvernChar = Cast<AWyvernCharacter>(OwnerComp.GetAIOwner()->GetPawn());

    if (WyvernChar)
    {
        WyvernChar->UpdateWalkSpeed(ChaseSpeed);
        AActor* Target = Cast<AActor>(TargetActor.SelectedKeyType);
       
        // AIMoveTo....
        OwnerComp.GetAIOwner()->MoveToActor(Target);
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}
