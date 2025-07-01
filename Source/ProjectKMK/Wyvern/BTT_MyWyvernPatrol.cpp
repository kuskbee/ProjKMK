// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MyWyvernPatrol.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "WyvernCharacter.h"
#include "NavigationSystem.h"
#include "Navigation\PathFollowingComponent.h"


EBTNodeResult::Type UBTT_MyWyvernPatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

	AWyvernCharacter* WyvernChar = Cast< AWyvernCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if (WyvernChar)
	{
		WyvernChar->UpdateWalkSpeed(PatrolSpeed);

		FNavLocation RandLocation;
		UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

		if (NavSys && NavSys->GetRandomReachablePointInRadius(
			WyvernChar->GetActorLocation(),
			PatrolRadius,
			RandLocation))
		{
			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalLocation(RandLocation);
			MoveRequest.SetAcceptanceRadius(PatrolRadius);

			FNavPathSharedPtr NavPath;
			EPathFollowingRequestResult::Type Result = OwnerComp.GetAIOwner()->MoveTo(MoveRequest, &NavPath);

			if (Result == EPathFollowingRequestResult::RequestSuccessful)
			{
				return EBTNodeResult::Succeeded;
			}
			else
			{
				return EBTNodeResult::Failed;
			}


		}
		else
		{
			return EBTNodeResult::Failed;
		}
	}
    return EBTNodeResult::Failed;
}
