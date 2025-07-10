// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_MyRunaway.h"
#include "Components/SkeletalMeshComponent.h"
#include "WyvernCharacter.h"
#include "MyDefine.h"

void UAnimNotify_MyRunaway::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AWyvernCharacter* WyvernCharacter = Cast<AWyvernCharacter>(MeshComp->GetOwner());

	if (IsValid(WyvernCharacter))
	{
		WyvernCharacter->MonAIState = EAIState::Runaway;
		WyvernCharacter->Phase = EPhase::SecondPhase;
	}
}
