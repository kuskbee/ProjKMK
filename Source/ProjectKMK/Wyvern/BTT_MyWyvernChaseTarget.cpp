// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MyWyvernChaseTarget.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "WyvernCharacter.h"
#include "Navigation\PathFollowingComponent.h"

EBTNodeResult::Type UBTT_MyWyvernChaseTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);


    AWyvernCharacter* WyvernChar = Cast<AWyvernCharacter>(OwnerComp.GetAIOwner()->GetPawn());

    if (WyvernChar)
    {
        WyvernChar->UpdateWalkSpeed(ChaseSpeed);
        AActor* Target = Cast<AActor>(TargetActor.SelectedKeyType);
       
        // AIMoveTo....
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalLocation(FVector(0, 0, 0));
		MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

		FNavPathSharedPtr NavPath;
		EPathFollowingRequestResult::Type Result = OwnerComp.GetAIOwner()->MoveTo(MoveRequest, &NavPath);

		if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			return EBTNodeResult::Succeeded;
		}
		else if (Result == EPathFollowingRequestResult::RequestSuccessful)
		{
			return EBTNodeResult::InProgress;
		}
		else if (Result == EPathFollowingRequestResult::Failed)
		{
			return EBTNodeResult::Failed;
		}
    }

    return EBTNodeResult::Failed;
}
