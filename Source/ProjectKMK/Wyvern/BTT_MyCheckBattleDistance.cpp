// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MyCheckBattleDistance.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "WyvernInterface.h"

EBTNodeResult::Type UBTT_MyCheckBattleDistance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* MyPawn = OwnerComp.GetAIOwner()->GetPawn();
	IWyvernInterface* Wyvern = Cast<IWyvernInterface>(MyPawn);

	AActor* Target = Cast<AActor>(TargetActor.SelectedKeyType);

	if (Wyvern && Target)
	{
		if (MyPawn->GetDistanceTo(Target) <= Distance)
		{
			Wyvern->SetAIState(EAIState::Battle);
			OwnerComp.GetBlackboardComponent()->SetValueAsEnum(TEXT("MonAIState"), (uint8) EAIState::Battle);
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
