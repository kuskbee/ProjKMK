// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_MyRightHandEnable.h"
#include "Components/SkeletalMeshComponent.h"
#include "WyvernCharacter.h"

void UAnimNotify_MyRightHandEnable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AWyvernCharacter* WyvernCharacter = Cast<AWyvernCharacter>(MeshComp->GetOwner());

	if (IsValid(WyvernCharacter))
	{
		WyvernCharacter->DoAttack(true, false);
	}
}
