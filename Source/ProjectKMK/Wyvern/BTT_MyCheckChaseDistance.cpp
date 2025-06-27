// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MyCheckChaseDistance.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "WyvernInterface.h"

EBTNodeResult::Type UBTT_MyCheckChaseDistance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* MyPawn = OwnerComp.GetAIOwner()->GetPawn();
	IWyvernInterface* Wyvern = Cast<IWyvernInterface>(MyPawn);

	AActor* Target = Cast<AActor>(TargetActor.SelectedKeyType);

	if (Wyvern && Target)
	{
		if (Distance < MyPawn->GetDistanceTo(Target))
		{
			Wyvern->SetAIState(EAIState::Chase);
			OwnerComp.GetBlackboardComponent()->SetValueAsEnum(TEXT("MonAIState"), (uint8)EAIState::Chase);
			return EBTNodeResult::Failed;
		}
		else
		{
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Succeeded;
}
