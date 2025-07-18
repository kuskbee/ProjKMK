// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdateAttackTarget.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "MyMonAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../PlayerCharacter.h"

UBTService_UpdateAttackTarget::UBTService_UpdateAttackTarget()
{
	Interval = 100.0f;
}

void UBTService_UpdateAttackTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AMyMonAIController* MonAIController = Cast<AMyMonAIController>(OwnerComp.GetAIOwner());
	ACharacter* WyvernChar = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (MonAIController && WyvernChar)
	{
		if (!WyvernChar->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
		{
			MonAIController->CheckTargetActors();
			MonAIController->ChangeTargetActor();
		}
	}
}
