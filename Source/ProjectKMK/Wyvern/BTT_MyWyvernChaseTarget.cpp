// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MyWyvernChaseTarget.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "WyvernCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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

            WyvernChar->GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
            return EBTNodeResult::Succeeded;
        }
    }

    return EBTNodeResult::Failed;
}
