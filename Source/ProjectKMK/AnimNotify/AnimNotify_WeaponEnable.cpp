// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_WeaponEnable.h"
#include "../PlayerCharacter.h"
#include "../Weapon/WeaponBase.h"

void UAnimNotify_WeaponEnable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;

    if (APlayerCharacter* Chr = Cast<APlayerCharacter>(MeshComp->GetOwner()))
    {
        if (AWeaponBase* Weapon = Chr->GetEquippedWeapon())
        {
            Weapon->SetWeaponCollisionEnable(true);
        }
    }
}
