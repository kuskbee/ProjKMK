// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MyChasePlayer.h"
#include "AIController.h"
#include "WyvernCharacter.h"

EBTNodeResult::Type UBTT_MyChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    AWyvernCharacter* WyvernChar = Cast<AWyvernCharacter>(OwnerComp.GetAIOwner()->GetPawn());

    if (WyvernChar)
    {
        WyvernChar->UpdateWalkSpeed(ChaseSpeed);
        return EBTNodeResult::Succeeded;
    }
    else
    {
        return EBTNodeResult::Failed;
    }
}
