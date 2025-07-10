// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MyWyvernPatrol.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "WyvernCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"


EBTNodeResult::Type UBTT_MyWyvernPatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

	AWyvernCharacter* WyvernChar = Cast<AWyvernCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if (WyvernChar)
	{
		WyvernChar->GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;

		FNavLocation RandomLocation;
		UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

		if (NavSys && NavSys->GetRandomReachablePointInRadius(
			WyvernChar->GetActorLocation(),
			PatrolRadius,
			RandomLocation))
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName(TEXT("NextPatrolPoint")),
				RandomLocation);
			return EBTNodeResult::Succeeded;
		}
	}

    return EBTNodeResult::Failed;
}
