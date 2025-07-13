// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_PlayerAttack.h"
#include "../PlayerCharacter.h"


void UAnimNotify_PlayerAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp) return;

    if (APlayerCharacter* Chr = Cast<APlayerCharacter>(MeshComp->GetOwner()))
    {
        Chr->Server_ExecuteAttack();
    }
}
