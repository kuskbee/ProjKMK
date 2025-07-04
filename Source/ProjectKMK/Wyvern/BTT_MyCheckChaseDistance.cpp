// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MyCheckChaseDistance.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "WyvernCharacter.h"

EBTNodeResult::Type UBTT_MyCheckChaseDistance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

	AWyvernCharacter* WyvernChar = Cast<AWyvernCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(FName(TEXT("TargetActor"))));

	if (WyvernChar && Target)
	{
		if (Distance < WyvernChar->GetDistanceTo(Target))
		{
			WyvernChar->MonAIState = EAIState::Chase;
			OwnerComp.GetBlackboardComponent()->SetValueAsEnum("MonAIState", uint8(EAIState::Chase));
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}
