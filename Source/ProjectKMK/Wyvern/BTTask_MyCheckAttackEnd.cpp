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

	ACharacter* MyCharacter = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (MyCharacter)
	{
		MyCharacter->GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &UBTTask_MyCheckAttackEnd::EventEndAttackMontage);
		MyCharacter->GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &UBTTask_MyCheckAttackEnd::EventEndAttackMontage);
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_MyCheckAttackEnd::EventEndAttackMontage(UAnimMontage* Montage, bool bInterrupted)
{
	if (CachedOwnerComp)
	{
		ACharacter* MyCharacter = Cast<ACharacter>(CachedOwnerComp->GetAIOwner()->GetPawn());
		if (MyCharacter)
		{
			MyCharacter->GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &UBTTask_MyCheckAttackEnd::EventEndAttackMontage);
			FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
		}
	}
}