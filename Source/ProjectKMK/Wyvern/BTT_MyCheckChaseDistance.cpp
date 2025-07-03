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

	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(FName(TEXT("TargetActor"))));

	if (Target)
	{
		if (Distance < MyPawn->GetDistanceTo(Target))
		{
			IWyvernInterface* Wyvern = Cast<IWyvernInterface>(MyPawn);
			Wyvern->SetAIState(EAIState::Chase);
			OwnerComp.GetBlackboardComponent()->SetValueAsEnum("MonAIState", uint8(EAIState::Chase));
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}
