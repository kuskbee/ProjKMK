// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_PlayerAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTKMK_API UAnimNotify_PlayerAttack : public UAnimNotify
{
	GENERATED_BODY()
	
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
	
};
