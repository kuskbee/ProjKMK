// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MyRotateToTarget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "AIController.h"
#include "WyvernCharacter.h"

UBTTask_MyRotateToTarget::UBTTask_MyRotateToTarget()
{
	//bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_MyRotateToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

	// Maybe this Code can Replaced To  Task "Wait"?
	FLatentActionInfo LatentInfo;
	UKismetSystemLibrary::Delay(GetWorld(), 0.75f, LatentInfo);
	return EBTNodeResult::Succeeded;
}

//void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
//{
//	if (OwnerComp.GetAIOwner())
//	{
//		AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));
//		
//		FRotator CurrentRot = OwnerComp.GetAIOwner()->GetPawn()->GetActorRotation();
//		FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation(),
//			Target->GetActorLocation());
//
//		FRotator LookRot = UKismetMathLibrary::RInterpTo(CurrentRot, TargetRot, DeltaSeconds, 5.0f); //RotationSpeed
//		OwnerComp.GetAIOwner()->GetPawn()->SetActorRotation(FRotator(0, 0, LookRot.Yaw));
//
//		FVector First = OwnerComp.GetAIOwner()->GetPawn()->GetActorForwardVector();
//		FVector Second = (OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation()) - (Target->GetActorLocation()).Normalize(0.0001);
//		float Dot = First.X * Second.X + First.Y + Second.Y + First.Z + Second.Z;
//
//		if (Dot >= 0.8f)
//		{
//			//return EBTNodeResult::Succeeded; Tick Task no return bb
//		}
//	}
//}