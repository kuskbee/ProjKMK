// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_MyCheckAttackEnd.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "WyvernCharacter.h"

EBTNodeResult::Type UBTTask_MyCheckAttackEnd::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	CachedOwnerComp = &OwnerComp;

	AWyvernCharacter* WyvernChar = Cast<AWyvernCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (WyvernChar)
	{
		WyvernChar->EventAttackEnd.RemoveDynamic(this, &UBTTask_MyCheckAttackEnd::EventEndAttackMontage);
		WyvernChar->EventAttackEnd.AddDynamic(this, &UBTTask_MyCheckAttackEnd::EventEndAttackMontage);
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_MyCheckAttackEnd::EventEndAttackMontage()
{
	if (CachedOwnerComp)
	{
		AWyvernCharacter* WyvernChar = Cast<AWyvernCharacter>(CachedOwnerComp->GetAIOwner()->GetPawn());
		if (WyvernChar)
		{
			WyvernChar->EventAttackEnd.RemoveDynamic(this, &UBTTask_MyCheckAttackEnd::EventEndAttackMontage);
			FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
		}
	}
}