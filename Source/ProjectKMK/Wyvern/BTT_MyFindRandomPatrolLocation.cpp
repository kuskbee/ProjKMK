// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MyFindRandomPatrolLocation.h"
#include "NavigationSystem.h"
#include "AI/Navigation/NavigationTypes.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "WyvernCharacter.h"


EBTNodeResult::Type UBTT_MyFindRandomPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AWyvernCharacter* WyvernChar = Cast< AWyvernCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if (WyvernChar)
	{
		WyvernChar->UpdateWalkSpeed(PatrolSpeed);

		FNavLocation ResultLocation;
		UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

		if (NavSys && NavSys->GetRandomReachablePointInRadius(
			WyvernChar->GetActorLocation(),
			PatrolRadius,
			ResultLocation))
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("MonAIState"), ResultLocation);
		}
		else
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("MonAIState"), WyvernChar->GetActorLocation());
		}
		return EBTNodeResult::Succeeded;

	}
	else
	{
		return EBTNodeResult::Failed;
	}
}
