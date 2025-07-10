// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MyCheckBattleDistance.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "WyvernCharacter.h"

EBTNodeResult::Type UBTT_MyCheckBattleDistance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->
		GetValueAsObject(TEXT("TargetActor")));

	if (Target)
	{
		AWyvernCharacter* WyvernChar = Cast<AWyvernCharacter>(OwnerComp.GetAIOwner()->GetPawn());

		if (WyvernChar)
		{
			FVector OwnerLocation = WyvernChar->GetActorLocation();
			float Offset = WyvernChar->GetDistanceTo(Target);

			if (Offset < Distance)
			{
				WyvernChar->MonAIState = EAIState::Battle;
				OwnerComp.GetBlackboardComponent()->SetValueAsEnum("MonAIState", uint8(EAIState::Battle));

				return EBTNodeResult::Succeeded;
			}
		}
	}
	else
	{
		// 만약 UBTService_UpdateAttackTarget의 tick의 간격을 줄이고, 어그로 시스템을 추가하면, 여기서 TargetActor가 Is Not Valid 했을때 MonAIState를 Change 해줄 필요가 없는데..
	}

	return EBTNodeResult::Failed;
}