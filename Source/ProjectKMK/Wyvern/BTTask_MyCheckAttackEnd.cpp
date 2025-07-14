// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_MyCheckAttackEnd.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../PlayerCharacter.h"
#include "MyMonAIController.h"

EBTNodeResult::Type UBTTask_MyCheckAttackEnd::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);


	APlayerCharacter* PlayerTarget = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));

	if (PlayerTarget)
	{
		if (PlayerTarget->EchoState == EPlayerState::EPS_Dead)
		{
			AMyMonAIController* WyvernController = Cast< AMyMonAIController>(OwnerComp.GetAIOwner());
			if (WyvernController)
			{
				WyvernController->RemoveTargetActor(PlayerTarget);
				WyvernController->ChangeTargetActor();
			}
		}
	}


	return EBTNodeResult::Succeeded;
}